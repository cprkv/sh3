#include "core/render/gapi/device.hpp"
#include "core/utils.hpp"
#include <dxgi1_6.h>

using namespace core::render;
using namespace core::render::gapi;

Device* core::render::gapi::gDevice = nullptr;


namespace
{
  // TODO: this happens because of sciter shit but is it worth it?? also need to use it when sciter comes
  bool sNativeLogState = true;


  void enablePrintingDebugMessages()
  {
    auto exceptionHandler = []( PEXCEPTION_POINTERS exceptionInfo ) -> LONG {
      if( !sNativeLogState )
        return EXCEPTION_CONTINUE_SEARCH;

      auto* exceptionRecord = exceptionInfo->ExceptionRecord;

      if( exceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_WIDE_C &&
          exceptionRecord->ExceptionCode != DBG_PRINTEXCEPTION_C )
        return EXCEPTION_CONTINUE_SEARCH;

      if( exceptionRecord->NumberParameters < 2 )
        return EXCEPTION_CONTINUE_SEARCH;

      auto stringLength = ( ULONG ) exceptionRecord->ExceptionInformation[0];
      auto stringPtr    = exceptionRecord->ExceptionInformation[1];

      if( exceptionRecord->ExceptionCode == DBG_PRINTEXCEPTION_C )
      {
        const auto* cstr = reinterpret_cast<const char*>( stringPtr );
        mCoreLogError( "[render] %s", cstr );
      }
      else
      {
        const auto* wcstr = reinterpret_cast<const wchar_t*>( stringPtr );
        auto        wstr  = std::wstring_view( wcstr, stringLength );
        auto        str   = convertWideStringToMultiByte( wstr );
        if( !str )
          return EXCEPTION_CONTINUE_SEARCH;
        mCoreLogError( "[render] %s", str->c_str() );
      }

      return EXCEPTION_CONTINUE_EXECUTION;
    };

    AddVectoredExceptionHandler( TRUE, exceptionHandler );
  }


  struct AdapterInfo
  {
    std::string           name;
    ComPtr<IDXGIAdapter1> adapter;
  };

  using AdaptersInfo = std::list<AdapterInfo>;

  Status getAdaptersInfo( IDXGIFactory1* factory, AdaptersInfo& out )
  {
    auto adapter = ComPtr<IDXGIAdapter1>();

    for( UINT adapterIndex = 0;
         gDevice->dxgiFactory->EnumAdapters1( adapterIndex, adapter.ReleaseAndGetAddressOf() ) != DXGI_ERROR_NOT_FOUND;
         adapterIndex++ )
    {
      auto desc = DXGI_ADAPTER_DESC{};
      mCoreCheckHR( adapter->GetDesc( &desc ) );
      out.emplace_back( AdapterInfo{
          .name    = convertWideStringToMultiByte( desc.Description ).value(),
          .adapter = std::move( adapter ),
      } );
    }

    if( !out.size() )
    {
      core::setErrorDetails( "no 3D adapter available" );
      return StatusSystemError;
    }

    return StatusOk;
  }

  AdapterInfo selectAdapter( const AdaptersInfo& adapters, const std::optional<std::string>& selectedGpu )
  {
    if( selectedGpu )
    {
      auto it = std::ranges::find_if( adapters, [&]( const auto& ai ) { return ai.name == *selectedGpu; } );
      if( it != adapters.end() )
      {
        mCoreLog( "selected adapter found: %s\n", selectedGpu->c_str() );
        return *it;
      }

      mCoreLogError( "selected gpu not found...\n" );
    }

    mCoreLog( "default adapter selected: %s\n", adapters.begin()->name.c_str() );
    return *adapters.begin();
  }
} // namespace


Status Device::init( HWND window )
{
  mCoreCheckHR( CreateDXGIFactory1( __uuidof( IDXGIFactory1 ), ( void** ) &dxgiFactory ) );

  // select adapter
  ComPtr<IDXGIAdapter1> adapter;
  {
    // TODO: get from config and save after selected
    auto adapters = AdaptersInfo();
    mCoreCheckStatus( getAdaptersInfo( dxgiFactory.Get(), adapters ) );
    auto adapterInfo = selectAdapter( adapters, "NVIDIA GeForce RTX 3050 Laptop GPU" );
    adapter          = std::move( adapterInfo.adapter );
  }

  // create device
  {
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;
    HMODULE         software   = nullptr;

    UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED; // TODO: debug layer not found on win11
#ifdef mCoreGAPIDeviceDebug
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    // flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // for sciter

    D3D_FEATURE_LEVEL featureLevels[]    = { D3D_FEATURE_LEVEL_11_0 };
    UINT              featureLevelsCount = ( UINT ) std::size( featureLevels );
    UINT              sdkVersion         = D3D11_SDK_VERSION;
    D3D_FEATURE_LEVEL featureLevel; // output

    mCoreCheckHR( D3D11CreateDevice(
        adapter.Get(), driverType, software, flags, featureLevels, featureLevelsCount, sdkVersion,
        &device, &featureLevel, &context ) );

    if( featureLevel != D3D_FEATURE_LEVEL_11_0 )
    {
      core::setErrorDetails( "can't create directx11 device: feature level doesn't match" );
      return StatusSystemError;
    }
  }

  // debug filter
  {
#ifdef mCoreGAPIDeviceDebug
    // TODO is this correct replacement for creating copy?
    // TODO is this really required???
    auto tmpDevice  = ComPtr<ID3D11Device>{ device };
    auto dx11Debug  = ComPtr<ID3D11Debug>{};
    auto debugQueue = ComPtr<ID3D11InfoQueue>{};

    mCoreCheckHR( tmpDevice.As( &dx11Debug ) );
    mCoreCheckHR( dx11Debug.As( &debugQueue ) );

    debugQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_CORRUPTION, true );
    debugQueue->SetBreakOnSeverity( D3D11_MESSAGE_SEVERITY_ERROR, true );
    debugQueue->PushEmptyStorageFilter();

    infoQueue = std::move( debugQueue );
#endif
  }

  // sampler states
  {
    mCoreCheckStatus( samplerStateWrap.init( D3D11_TEXTURE_ADDRESS_WRAP ) );
    mCoreCheckStatus( samplerStateClamp.init( D3D11_TEXTURE_ADDRESS_CLAMP ) );
  }

  // blend states
  {
    {
      auto defaultTargetBlendDesc = D3D11_RENDER_TARGET_BLEND_DESC{
          .BlendEnable           = FALSE,
          .SrcBlend              = D3D11_BLEND_ONE,
          .DestBlend             = D3D11_BLEND_ZERO,
          .BlendOp               = D3D11_BLEND_OP_ADD,
          .SrcBlendAlpha         = D3D11_BLEND_ONE,
          .DestBlendAlpha        = D3D11_BLEND_ZERO,
          .BlendOpAlpha          = D3D11_BLEND_OP_ADD,
          .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
      };
      auto defaultBlendDesc = D3D11_BLEND_DESC{
          .AlphaToCoverageEnable  = false,
          .IndependentBlendEnable = false,
          .RenderTarget           = { defaultTargetBlendDesc },
      };
      mCoreCheckHR( device->CreateBlendState( &defaultBlendDesc, &blendStateDefault ) );
    }

    {
      auto alphaTargetBlendDesc = D3D11_RENDER_TARGET_BLEND_DESC{
          .BlendEnable           = TRUE,
          .SrcBlend              = D3D11_BLEND_SRC_ALPHA,
          .DestBlend             = D3D11_BLEND_INV_SRC_ALPHA,
          .BlendOp               = D3D11_BLEND_OP_ADD,
          .SrcBlendAlpha         = D3D11_BLEND_ZERO,
          .DestBlendAlpha        = D3D11_BLEND_ONE,
          .BlendOpAlpha          = D3D11_BLEND_OP_ADD,
          .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
      };
      auto alphaBlendDesc = D3D11_BLEND_DESC{
          .AlphaToCoverageEnable  = false,
          .IndependentBlendEnable = false,
          .RenderTarget           = { alphaTargetBlendDesc },
      };
      mCoreCheckHR( device->CreateBlendState( &alphaBlendDesc, &blendStateAlpha ) );
    }
  }

#ifdef mCoreGAPIDeviceDebug
  enablePrintingDebugMessages();
#endif

  mCoreCheckStatus( viewport.init( window ) );
  return StatusOk;
}


Status Device::logMessages()
{
#ifdef mCoreGAPIDeviceDebug
  if( !IsDebuggerPresent() )
    return {};

  UINT64 messageCount = infoQueue->GetNumStoredMessages();

  for( UINT64 i = 0; i < messageCount; ++i )
  {
    SIZE_T messageSize = 0;
    infoQueue->GetMessage( i, nullptr, &messageSize );

    auto message    = std::make_unique<u8[]>( messageSize );
    auto messagePtr = reinterpret_cast<D3D11_MESSAGE*>( message.get() );
    mCoreCheckHR( infoQueue->GetMessage( i, messagePtr, &messageSize ) ); // get the actual message

    if( messagePtr->Severity == D3D11_MESSAGE_SEVERITY_INFO )
    {
      continue; // TODO: too many info logs about state from sciter
    }

    const char* category = "<unknown>";
    const char* severity = "<unknown>";

    switch( messagePtr->Category )
    {
      case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:
        category = "MISCELLANEOUS";
        break;
      case D3D11_MESSAGE_CATEGORY_INITIALIZATION:
        category = "INITIALIZATION";
        break;
      case D3D11_MESSAGE_CATEGORY_CLEANUP:
        category = "CLEANUP";
        break;
      case D3D11_MESSAGE_CATEGORY_COMPILATION:
        category = "COMPILATION";
        break;
      case D3D11_MESSAGE_CATEGORY_STATE_CREATION:
        category = "STATE_CREATION";
        break;
      case D3D11_MESSAGE_CATEGORY_STATE_SETTING:
        category = "STATE_SETTING";
        break;
      case D3D11_MESSAGE_CATEGORY_STATE_GETTING:
        category = "STATE_GETTING";
        break;
      case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
        category = "RESOURCE_MANIPULATION";
        break;
      case D3D11_MESSAGE_CATEGORY_EXECUTION:
        category = "EXECUTION";
        break;
      case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:
        category = "APPLICATION_DEFINED";
        break;
      case D3D11_MESSAGE_CATEGORY_SHADER:
        category = "SHADER";
        break;
    }

    switch( messagePtr->Severity )
    {
      case D3D11_MESSAGE_SEVERITY_CORRUPTION:
        severity = "CORRUPTION";
        break;
      case D3D11_MESSAGE_SEVERITY_ERROR:
        severity = "ERROR";
        break;
      case D3D11_MESSAGE_SEVERITY_WARNING:
        severity = "WARNING";
        break;
      case D3D11_MESSAGE_SEVERITY_INFO:
        severity = "INFO";
        break;
      case D3D11_MESSAGE_SEVERITY_MESSAGE:
        category = "MESSAGE";
        break;
    }

    mCoreLog( "[render][%s][%s] %.*s", severity, category, ( int ) messagePtr->DescriptionByteLength, messagePtr->pDescription );
  }

  infoQueue->ClearStoredMessages();
#endif

  return StatusOk;
}


void Device::enableAlphaBlending( bool enable )
{
  const u32 mask = 0xffffffffu;

  if( enable )
  {
    float blendFactor[4] = { 0, 0, 0, 0 };
    context->OMSetBlendState( blendStateAlpha.Get(), blendFactor, mask );
  }
  else
  {
    context->OMSetBlendState( nullptr, nullptr, mask );
  }
}

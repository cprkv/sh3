#include "core/common.hpp"
#include "core/render/gapi/common.hpp"
#include "core/render/gapi/shader-compiler.hpp"
#include "core/fs/file.hpp"
#include "core/fs/path.hpp"
#include <d3dcompiler.h>

using namespace core;
using namespace core::render;
using namespace core::render::gapi;


namespace
{
  const char* shaderTypeToProfileName( ShaderType shaderType )
  {
    switch( shaderType )
    {
      case ShaderTypeVertex:
        return "vs_5_0";
      case ShaderTypePixel:
        return "ps_5_0";
    }
  }


  struct IncludeHandler : public ID3DInclude
  {
    std::string directory_;

    explicit IncludeHandler( std::string directory )
        : directory_{ std::move( directory ) }
    {}

    virtual ~IncludeHandler() = default;

    __declspec( nothrow ) HRESULT __stdcall Open(
        D3D_INCLUDE_TYPE includeType,
        LPCSTR           pFileName,
        LPCVOID          pParentData,
        LPCVOID*         ppData,
        UINT*            pBytes ) override
    {
      ( void ) includeType;
      ( void ) pParentData;

      auto path = core::fs::pathJoin( directory_, pFileName );
      auto f    = core::fs::File{ path.c_str(), "rb" };

      if( !f.isOpen() || !f.getSize() )
      {
        mCoreLogError( "error open shader include file: '%s'\n", pFileName );
        return E_UNEXPECTED;
      }

      auto* bytes = new byte[f.getSize()];

      if( f.read( bytes, f.getSize() ) != StatusOk )
      {
        mCoreLogError( "error read shader include file: '%s'\n", pFileName );
        delete[] bytes;
        return E_UNEXPECTED;
      }

      *ppData = bytes;
      *pBytes = static_cast<UINT>( f.getSize() );

      return S_OK;
    }

    __declspec( nothrow ) HRESULT Close( LPCVOID pData ) override
    {
      if( !pData )
      {
        mCoreLog( "warning: IncludeHandler::Close called with nullptr\n" );
        return S_OK;
      }

      const char* cData = static_cast<const char*>( pData );
      char*       data  = const_cast<char*>( cData );
      delete[] data;
      return S_OK;
    }
  };
} // namespace


Status render::gapi::compileShader( const char*      directory,
                                    const char*      name,
                                    ShaderType       shaderType,
                                    std::vector<u8>& out )
{
  auto bytes = std::vector<byte>();
  mCoreCheckStatus( fs::readFile( fs::pathJoin( directory, name ), bytes ) );

  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS |
               D3DCOMPILE_OPTIMIZATION_LEVEL3 |
               D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifdef _DEBUG
  flags |= D3DCOMPILE_DEBUG;
#endif

  const D3D_SHADER_MACRO defines[] = { { nullptr, nullptr } };

  auto shaderBlob = ComPtr<ID3DBlob>{};
  auto errorBlob  = ComPtr<ID3DBlob>{};

  const char* profile        = shaderTypeToProfileName( shaderType );
  auto        includeHandler = IncludeHandler{ directory };

  auto hrr = ::D3DCompile( bytes.data(), bytes.size(),
                           name, defines, &includeHandler,
                           "main", profile, flags, 0, &shaderBlob, &errorBlob );

  if( errorBlob.Get() && errorBlob->GetBufferSize() )
  {
    auto errorMessage = std::string_view( static_cast<char*>( errorBlob->GetBufferPointer() ),
                                          errorBlob->GetBufferSize() );
    core::setErrorDetails( "error compiling shader: " mFmtS, mFmtSValue( errorMessage ) );
    return StatusSystemError;
  }

  if( !shaderBlob.Get() || !shaderBlob->GetBufferSize() )
  {
    core::setErrorDetails( "unknown error happend while compiling shader" );
    return StatusSystemError;
  }

  mCoreCheckHR( hrr );

  out = std::vector<u8>(
      static_cast<u8*>( shaderBlob->GetBufferPointer() ),
      static_cast<u8*>( shaderBlob->GetBufferPointer() ) + shaderBlob->GetBufferSize() );
  return StatusOk;
}

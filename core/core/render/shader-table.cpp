#include "core/render/shader-table.hpp"

using namespace core::render;


namespace
{
  template<typename... TVertexShaderItemLayout>
  Status makePipelineFromSource(
      ShaderPipeline&    out,
      const std::string& directory,
      const char*        vertexSourceName,
      const char*        pixelSourceName,
      TVertexShaderItemLayout... vertexShaderItemLayouts )
  {
    gapi::VertexShaderItemLayout layout[] = { vertexShaderItemLayouts... };
    mCoreCheckStatus( out.vertexShader.initFromSource(
        directory.c_str(), vertexSourceName, gapi::VertexShaderLayout( layout ) ) );
    mCoreCheckStatus( out.pixelShader.initFromSource(
        directory.c_str(), pixelSourceName ) );
    return StatusOk;
  }
} // namespace


Status ShaderTable::init( std::string initDirectory )
{
  directory = std::move( initDirectory );
  return reload();
}


#define mShaderPair( x ) #x ".vs.hlsl", #x ".ps.hlsl"

Status ShaderTable::reload()
{
  using L = gapi::VertexShaderItemLayout;
  ShaderTable st;

  mCoreCheckStatus( makePipelineFromSource( st.texture2D, directory, mShaderPair( texture_2d ),
                                            L{ .name = "Position", .format = gapi::GPUFormatRG32F },
                                            L{ .name = "UV", .format = gapi::GPUFormatRG32F } ) );

  mCoreCheckStatus( makePipelineFromSource( st.texture2DMS, directory, "texture_2d.vs.hlsl", "texture_2dms.ps.hlsl",
                                            L{ .name = "Position", .format = gapi::GPUFormatRG32F },
                                            L{ .name = "UV", .format = gapi::GPUFormatRG32F } ) );

  mCoreCheckStatus( makePipelineFromSource( st.oldFull, directory, mShaderPair( old_full ),
                                            L{ .name = "MyPosition", .format = gapi::GPUFormatRGB32F },
                                            L{ .name = "Normal", .format = gapi::GPUFormatRGB32F },
                                            L{ .name = "UVCoord", .format = gapi::GPUFormatRG32F } ) );

  mCoreCheckStatus( makePipelineFromSource( st.line, directory, mShaderPair( line ),
                                            L{ .name = "LinePosition", .format = gapi::GPUFormatRGB32F } ) );

  *this = st;
  return StatusOk;
}

#undef mShaderPair

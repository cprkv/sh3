#include "image.hpp"
#include "core/data/string-id.hpp"
#include "core/data/data.hpp"
#include "core/fs/file.hpp"
#include <nvtt/nvtt.h>
#include <DirectXTex.h>

using namespace intermediate;
namespace dx = DirectX;


#define mFailIfHr( ... )                                             \
  if( HRESULT hr{ __VA_ARGS__ }; FAILED( hr ) )                      \
  {                                                                  \
    printf( "Expression = " #__VA_ARGS__ " failed:\nHRESULT %08X\n", \
            static_cast<unsigned int>( hr ) );                       \
    abort();                                                         \
  }


#define mNotImplementedIf( ... )                                       \
  if( __VA_ARGS__ )                                                    \
  {                                                                    \
    printf( "feature for " #__VA_ARGS__ " is not yet implemented\n" ); \
    abort();                                                           \
  }


#define mFailIf( ... )                                                 \
  if( __VA_ARGS__ )                                                    \
  {                                                                    \
    printf( "fatal error: expression (" #__VA_ARGS__ ") is true!\n" ); \
    abort();                                                           \
  }


namespace
{
  // all intermediate data stored here
  struct TmpTexture
  {
    TextureInfo                 info;
    std::string                 intermediatePath;
    core::data::schema::Texture result;
  };

  u64 textureHash( const TextureInfo& textureInfo )
  {
    auto path = stdfs::path( textureInfo.path.path ).replace_extension( "" ).generic_string();
    printf( "texture path: %s\n", path.c_str() );
    return core::data::StringId( path );
  }

  bool textureEq( const TextureInfo& a, const TextureInfo& b )
  {
    return a.path.path == b.path.path;
  }


  std::string getTextureIntermediateOutputPath( const TextureInfo& texture )
  {
    auto texturePath           = stdfs::path( core::data::getDataPath( texture.path.path.c_str() ) );
    auto newName               = texturePath.filename().replace_extension( ".dds" );
    auto intermediateDirectory = texturePath.parent_path() / ".intermediate";
    stdfs::create_directories( intermediateDirectory );
    return ( intermediateDirectory / newName ).generic_string();
  }


  enum class ImageUsage
  {
    AlbedoOpaque,
    AlbedoTransparent,
    AlbedoPerforating,
    Normal,
    Roughness,
  };


  void convertToDds( TmpTexture& texture, ImageUsage usage = ImageUsage::AlbedoOpaque )
  {
    printf( "converting texture:\n  %s ->\n  %s\n",
            texture.info.path.full.c_str(), texture.intermediatePath.c_str() );

    nvtt::Surface image;
    mFailIf( !image.load( texture.info.path.full.c_str() ) );

    const bool enableCuda         = true;
    auto       context            = nvtt::Context{ enableCuda };
    auto       compressionOptions = nvtt::CompressionOptions{};

    if( usage == ImageUsage::AlbedoOpaque )
    {
      compressionOptions.setFormat( nvtt::Format_BC1 );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    // TODO: separate AlbedoPerforating
    else if( usage == ImageUsage::AlbedoTransparent || usage == ImageUsage::AlbedoPerforating )
    {
      compressionOptions.setFormat( nvtt::Format_BC1a );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    else if( usage == ImageUsage::Normal )
    {
      image.setNormalMap( true );
      compressionOptions.setFormat( nvtt::Format_BC7 );
      compressionOptions.setPixelFormat( 7, 7, 7, 0 );
      compressionOptions.setQuality( nvtt::Quality_Normal );
    }
    else if( usage == ImageUsage::Roughness )
    {
      // TODO: this is just random stuff for fulfill emptiness, need to fix to correct format, like 2 channel image...
      compressionOptions.setFormat( nvtt::Format_BC1 );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    else
    {
      printf( "unknown usage attribute\n" );
      abort();
    }

    image.ToGPU();

    auto outputOptions = nvtt::OutputOptions{};
    outputOptions.setFileName( texture.intermediatePath.c_str() );

    const int numMipmaps = image.countMipmaps();

    if( !context.outputHeader( image, numMipmaps, compressionOptions, outputOptions ) )
    {
      printf( "error writing dds header\n" );
      abort();
    }

    for( int mip = 0; mip < numMipmaps; ++mip )
    {
      if( !context.compress( image, 0 /* face */, mip, compressionOptions, outputOptions ) )
      {
        printf( "error compressing and writing the DDS file\n" );
        abort();
      }

      if( mip == numMipmaps - 1 )
        break;

      // prepare next mip

      // Convert to linear premultiplied alpha. Note that toLinearFromSrgb()
      // will clamp HDR images; consider e.g. toLinear(2.2f) instead.
      image.toLinearFromSrgb();
      image.premultiplyAlpha();

      // Resize the image to the next mipmap size.
      image.buildNextMipmap( nvtt::MipmapFilter_Kaiser );

      if( image.isNormalMap() )
        image.normalizeNormalMap();

      // Convert back to unpremultiplied sRGB.
      image.demultiplyAlpha();
      image.toSrgb();
    }
  }


  void loadTexture( TmpTexture& texture )
  {
    auto bytes = std::vector<byte>();
    if( core::fs::readFile( texture.intermediatePath.c_str(), bytes ) != StatusOk )
    {
      printf( "error: bad file: %s\n", texture.intermediatePath.c_str() );
      abort();
    }

    auto metaData = dx::TexMetadata();
    mFailIfHr( dx::GetMetadataFromDDSMemory( bytes.data(), bytes.size(), dx::DDS_FLAGS_NONE, metaData ) );

    mNotImplementedIf( metaData.IsVolumemap() );
    mNotImplementedIf( metaData.IsCubemap() );
    mNotImplementedIf( metaData.arraySize != 1 );
    mNotImplementedIf( metaData.dimension != DirectX::TEX_DIMENSION_TEXTURE2D );
    mNotImplementedIf( metaData.format == DXGI_FORMAT_UNKNOWN );

    auto image = dx::ScratchImage();
    mFailIfHr( dx::LoadFromDDSMemory( bytes.data(), bytes.size(), dx::DDS_FLAGS_NONE, &metaData, image ) );

    texture.result.data = std::vector<core::data::schema::TextureData>( metaData.mipLevels * metaData.arraySize );

    {
      const auto* srcImages = image.GetImages();
      auto        numImages = image.GetImageCount();

      size_t idx = 0;
      for( size_t item = 0; item < metaData.arraySize; ++item )
      {
        for( size_t level = 0; level < metaData.mipLevels; ++level )
        {
          size_t index = metaData.ComputeIndex( level, item, 0 );
          mFailIf( index >= numImages );

          const auto& img = srcImages[index];

          mFailIf( img.format != metaData.format );
          mFailIf( !img.pixels );
          mFailIf( idx >= ( metaData.mipLevels * metaData.arraySize ) );
          mFailIf( !img.slicePitch );

          texture.result.data[idx].mem           = { img.pixels, img.pixels + img.slicePitch };
          texture.result.data[idx].memPitch      = static_cast<u32>( img.rowPitch );
          texture.result.data[idx].memSlicePitch = static_cast<u32>( img.slicePitch );
          ++idx;
        }
      }
    }

    texture.result.width     = static_cast<u32>( metaData.width );
    texture.result.height    = static_cast<u32>( metaData.height );
    texture.result.mipLevels = static_cast<u32>( metaData.mipLevels );
    texture.result.arraySize = static_cast<u32>( metaData.arraySize );
    texture.result.format    = metaData.format;
  }
} // namespace


void intermediate::processMaterials( const SceneInfo&           sceneInfo,
                                     core::data::schema::Chunk& outputChunk )
{
  std::map<u64, TmpTexture> textures;

  for( const auto& mesh: sceneInfo.meshes )
  {
    u64 hash = textureHash( mesh.material_info.diffuse );
    if( auto it = textures.find( hash );
        it != textures.end() )
    {
      if( !textureEq( it->second.info, mesh.material_info.diffuse ) )
      {
        printf( "ERROR: different textures has same texture ids\n" );
        abort();
      }
    }
    else
    {
      auto intermediatePath = getTextureIntermediateOutputPath( mesh.material_info.diffuse );
      auto tmpTexture       = TmpTexture{
                .info             = mesh.material_info.diffuse,
                .intermediatePath = std::move( intermediatePath ),
                .result           = { .id = hash },
      };
      textures.emplace( hash, std::move( tmpTexture ) );
    }
  }

  // 1. convert textures (nvtt)
  for( auto& [_, texture]: textures )
  {
    // TODO: pass usage (check material settings)
    convertToDds( texture );
  }

  // 2. load all textures in memory (DirectXTexSimpl)
  for( auto& [_, texture]: textures )
  {
    loadTexture( texture );
  }

  // 3. write textures and materials to outputChunk
  for( auto& [_, texture]: textures )
  {
    outputChunk.textures.push_back( texture.result );
  }

  for( const auto& mesh: sceneInfo.meshes )
  {
    u64  materialId = core::data::StringId( mesh.material_info.name );
    auto materialIt = std::find_if(
        outputChunk.materials.begin(), outputChunk.materials.end(),
        [=]( const core::data::schema::Material& m ) { return m.id == materialId; } );

    if( materialIt == outputChunk.materials.end() )
    {
      outputChunk.materials.emplace_back( core::data::schema::Material{
          .id               = materialId,
          .diffuseTextureId = textureHash( mesh.material_info.diffuse ),
      } );
    }
  }
}
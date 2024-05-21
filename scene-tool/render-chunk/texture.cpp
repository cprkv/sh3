#include "render-chunk/texture.hpp"
#include "scene-tool.hpp"
#include "util/detect-changes.hpp"
#include "core/core.hpp"
#include <nvtt/nvtt.h>
#include <DirectXTex.h>

using namespace intermediate;
namespace dx = DirectX;


namespace
{
  // all intermediate data stored here
  struct TmpTexture
  {
    TextureInfo                 info;
    core::render::BlendMode     blendMode;
    std::string                 intermediatePath;
    core::data::schema::Texture result;
  };

  bool operator==( const TextureInfo& a, const TextureInfo& b ) { return a.path == b.path; }
  bool operator!=( const TextureInfo& a, const TextureInfo& b ) { return !operator==( a, b ); }


  std::string getTextureIntermediateOutputPath( const TextureInfo& texture )
  {
    auto texturePath           = stdfs::path( core::data::getDataPath( stdfs::path( texture.path ) ) );
    auto newName               = texturePath.filename().replace_extension( ".dds" );
    auto intermediateDirectory = texturePath.parent_path() / ".intermediate";
    stdfs::create_directories( intermediateDirectory );
    return ( intermediateDirectory / newName ).generic_string();
  }


  void convertToDds( TmpTexture& texture )
  {
    auto fullPath = getResourcePath( texture.info.path );

    if( !stdfs::exists( fullPath ) )
    {
      printf( "not found: %s\n", fullPath.string().c_str() );
      abort();
    }

    mFailIf( !stdfs::is_regular_file( fullPath ) );

    printf( "texture (%s):\n  %s ->\n  %s\n", core::render::toString( texture.blendMode ),
            fullPath.string().c_str(),
            texture.intermediatePath.c_str() );

    auto changes = FileChanges( fullPath, texture.intermediatePath );
    if( !changes.isChanged() )
      return;

    printf( "has changes, converting...\n" );

    nvtt::Surface image;
    mFailIf( !image.load( fullPath.string().c_str() ) );

    const bool enableCuda         = true;
    auto       context            = nvtt::Context{ enableCuda };
    auto       compressionOptions = nvtt::CompressionOptions{};

    if( texture.blendMode == core::render::BlendMode_Opaque )
    {
      compressionOptions.setFormat( nvtt::Format_DXT1 );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    else if( texture.blendMode == core::render::BlendMode_AlphaHash )
    {
      compressionOptions.setFormat( nvtt::Format_DXT1a );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    else if( texture.blendMode == core::render::BlendMode_AlphaBlend )
    {
      compressionOptions.setFormat( nvtt::Format_DXT3 );
      compressionOptions.setQuality( nvtt::Quality_Production );
    }
    //else if( texture.blendMode == core::render::BlendMode_Normal )
    //{
    //  image.setNormalMap( true );
    //  compressionOptions.setFormat( nvtt::Format_BC7 ); // ???
    //  compressionOptions.setPixelFormat( 7, 7, 7, 0 );
    //  compressionOptions.setQuality( nvtt::Quality_Normal );
    //}
    //else if( texture.blendMode == core::render::BlendMode_Roughness )
    //{
    //  // TODO: this is just random stuff for fulfill emptiness, need to fix to correct format, like 2 channel image...
    //  compressionOptions.setFormat( nvtt::Format_BC1 );
    //  compressionOptions.setQuality( nvtt::Quality_Production );
    //}
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

    changes.markChanged();
  }


  core::render::BlendMode chooseMoreAdvancedBlendMode( core::render::BlendMode a, core::render::BlendMode b )
  {
    using U = std::underlying_type_t<core::render::BlendMode>;
    return static_cast<core::render::BlendMode>(
        std::max( static_cast<U>( a ),
                  static_cast<U>( b ) ) );
  }


  void loadTexture( TmpTexture& texture )
  {
    printf( "loading texture %s...\n", texture.intermediatePath.c_str() );

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
    texture.result.format    = static_cast<u32>( metaData.format );
  }
} // namespace


struct TextureCollection : public ITextureCollection
{
  std::map<u64, TmpTexture> textures_;

  ~TextureCollection() override = default;


  void addTexture( const TextureInfo& textureInfo, const std::string& blendModeStr ) override
  {
    u64 hash = textureHash( textureInfo );

    if( auto it = textures_.find( hash ); it != textures_.end() )
    {
      if( it->second.info != textureInfo )
      {
        printf( "ERROR: different textures has same texture ids\n" );
        abort();
      }

      auto blendMode       = parseBlendMode( blendModeStr );
      it->second.blendMode = chooseMoreAdvancedBlendMode( it->second.blendMode, blendMode );
      return;
    }

    auto intermediatePath = getTextureIntermediateOutputPath( textureInfo );
    auto blendMode        = parseBlendMode( blendModeStr );

    auto tmpTexture = TmpTexture{
        .info             = textureInfo,
        .blendMode        = blendMode,
        .intermediatePath = std::move( intermediatePath ),
        .result           = { .id = hash },
    };
    textures_.emplace( hash, std::move( tmpTexture ) );
  }


  void process() override
  {
    // 1. convert textures (nvtt)
    for( auto& [_, texture]: textures_ )
    {
      // TODO: pass usage (check material settings)
      convertToDds( texture );
    }

    // 2. load all textures in memory (DirectXTexSimpl)
    for( auto& [_, texture]: textures_ )
    {
      loadTexture( texture );
    }
  }


  void resolve( const SceneInfo& sceneInfo, core::data::schema::Chunk& outputChunk ) override
  {
    auto materials = sceneInfo.objects |
                     std::ranges::views::filter( []( const ObjectInfo& o ) { return o.mesh.has_value(); } ) |
                     std::ranges::views::transform( []( const ObjectInfo& o ) { return o.mesh->material_info; } ) |
                     std::ranges::to<std::vector>();

    auto eraseMaterials = std::ranges::unique(
        materials, []( const MaterialInfo& a, const MaterialInfo& b ) { return a.name == b.name; } );
    materials.erase( eraseMaterials.begin(), eraseMaterials.end() );

    auto diffuseTextures = materials |
                           std::ranges::views::transform( []( const MaterialInfo& m ) { return textureHash( m.diffuse ); } ) |
                           std::ranges::to<std::set>();

    for( auto textureId: diffuseTextures )
    {
      printf( "add image to render chunk: " mFmtU64 "\n", textureId );
      outputChunk.textures.push_back( textures_.at( textureId ).result );
    }
  }
};


std::unique_ptr<ITextureCollection> intermediate::makeTextureCollection()
{
  return std::make_unique<TextureCollection>();
}

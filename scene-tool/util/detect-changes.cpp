#include "detect-changes.hpp"
#include "scene-tool.hpp"
#include <wincrypt.h>

using namespace intermediate;

namespace
{
  struct StaticData
  {
    HCRYPTPROV cryptProv;
  };

  StaticData* sData = nullptr;

  void ensureStaticData()
  {
    if( sData ) return;
    sData = new StaticData();
    mFailIfWinApi( !CryptAcquireContext( &sData->cryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) );
  }

  std::string md5( std::span<byte> bytes )
  {
    ensureStaticData();

    HCRYPTHASH cryptHash;
    mFailIfWinApi( !CryptCreateHash( sData->cryptProv, CALG_MD5, 0, 0, &cryptHash ) );

    mFailIfWinApi( !CryptHashData( cryptHash, static_cast<BYTE*>( bytes.data() ), static_cast<DWORD>( bytes.size() ), 0 ) );

    BYTE  hash[16] = { 0 };
    DWORD hashSize = 16;
    mFailIfWinApi( !CryptGetHashParam( cryptHash, HP_HASHVAL, hash, &hashSize, 0 ) );

    const char* hex     = "0123456789abcdef";
    auto        strHash = std::string( 32, '\0' );
    for( unsigned i = 0; i < hashSize; ++i )
    {
      strHash[i * 2]         = hex[hash[i] >> 4];
      strHash[( i * 2 ) + 1] = hex[hash[i] & 0xF];
    }

    CryptDestroyHash( cryptHash );
    return strHash;
  }
} // namespace


FileChanges::FileChanges( const stdfs::path& sourceFilePath,
                          const stdfs::path& destinationFilePath )
{
  auto bytes = std::vector<byte>();
  mFailIf( core::fs::readFile( sourceFilePath.string(), bytes ) != StatusOk );
  hash_ = md5( bytes );

  auto hashFile = destinationFilePath;
  hashFile += ".source-hash";
  hashPath_ = hashFile.string();

  if( !stdfs::exists( hashFile ) )
  {
    changed_ = true;
    printf( "file %s changed (no hash hint)\n", sourceFilePath.string().c_str() );
  }
  else
  {
    auto oldHashBytes = std::vector<byte>();
    mFailIf( core::fs::readFile( hashPath_, oldHashBytes ) != StatusOk );
    auto oldHash = std::string_view( reinterpret_cast<char*>( oldHashBytes.data() ), oldHashBytes.size() );
    changed_     = oldHash != hash_;
    if( changed_ )
      printf( "file %s changed (hash mismatch)\n", sourceFilePath.string().c_str() );
    else
      printf( "file %s is still the same\n", sourceFilePath.string().c_str() );
  }
}

bool FileChanges::isChanged() const
{
  return changed_;
}

void FileChanges::markChanged() const
{
  auto bytes = std::vector<byte>(
      reinterpret_cast<const byte*>( hash_.c_str() ),
      reinterpret_cast<const byte*>( hash_.c_str() + strlen( hash_.c_str() ) ) );
  mFailIf( core::fs::writeFile( hashPath_, bytes ) );
}

#include "core/fs/file.hpp"

using namespace core;
using namespace core::fs;


File::File( const char* path, const char* mode )
{
  stream_ = fopen( path, mode );

  if( !stream_ )
  {
    core::setErrorDetails( "can't open file '%s'", path );
    return;
  }

  if( fseek( stream_, 0, SEEK_END ) )
  {
    core::setErrorDetails( "can't get file size '%s': fseek at end failed", path );
    return;
  }

  long pos = ftell( stream_ );
  if( pos < 0 )
  {
    core::setErrorDetails( "can't get file size '%s': ftell failed", path );
    fclose( stream_ );
    size_   = 0;
    stream_ = nullptr;
    return;
  }

  size_ = static_cast<size_t>( pos );

  if( fseek( stream_, 0, SEEK_SET ) )
  {
    core::setErrorDetails( "can't get file size '%s': fseek at begin failed", path );
    fclose( stream_ );
    size_   = 0;
    stream_ = nullptr;
  }
}


File::~File()
{
  if( stream_ )
    fclose( stream_ );
}


bool File::isEOF()
{
  if( !stream_ )
    return true;

  return feof( stream_ );
}


Status File::seek( FileSeekDirection direction, long offset )
{
  if( !stream_ )
    return StatusBadFile;

  int directionNative;

  if( direction == FileSeekDirectionBegin )
    directionNative = SEEK_SET;
  else if( direction == FileSeekDirectionEnd )
    directionNative = SEEK_END;
  else
    directionNative = SEEK_SET;

  int r = fseek( stream_, offset, directionNative );
  if( r )
  {
    core::setErrorDetails( "seek failed" );
    return StatusSystemError;
  }

  return StatusOk;
}


size_t File::getPosition()
{
  if( !stream_ ) return 0;
  return static_cast<size_t>( ftell( stream_ ) );
}


Status File::write( const void* buffer, size_t size )
{
  if( !stream_ )
    return StatusBadFile;

  size_t elementsWritten = fwrite( buffer, size, 1, stream_ );
  if( elementsWritten != 1 )
    return StatusSystemError;

  return StatusOk;
}


Status File::read( void* buffer, size_t size )
{
  if( !stream_ )
    return StatusBadFile;

  if( size == 0 )
    return StatusOk;

  size_t elementsRead = fread( buffer, size, 1, stream_ );
  if( elementsRead != 1 )
    return StatusSystemError;

  return StatusOk;
}


Status fs::readFile( const char* path, std::vector<u8>& out )
{
  auto f = File{ path, "rb" };
  out.resize( f.getSize() );
  return f.read( out.data(), out.size() );
}


Status fs::getEntryInfo( const stdfs::path& path, EntryInfo& out )
{
  auto ec = std::error_code();

  auto entry = stdfs::directory_entry( path, ec );
  if( ec )
  {
    core::setErrorDetails( "getEntryInfo: init entry: %s", ec.message().c_str() );
    return StatusSystemError;
  }

  auto status = entry.status( ec );
  if( ec )
  {
    if( ec == std::errc::no_such_file_or_directory )
    {
      return StatusNotFound;
    }

    core::setErrorDetails( "getEntryInfo: check status: %s", ec.message().c_str() );
    return StatusSystemError;
  }

  if( status.type() == stdfs::file_type::not_found ||
      status.type() == stdfs::file_type::none )
  {
    return StatusNotFound;
  }

  switch( status.type() )
  {
    case stdfs::file_type::regular:
      out.type = FsEntryTypeFile;
      break;
    case stdfs::file_type::directory:
      out.type = FsEntryTypeDirectory;
      break;
    default:
      core::setErrorDetails( "getEntryInfo: not a file or directory" );
      return StatusSystemError;
  }

  return StatusOk;
}


Status fs::findFileUp( const stdfs::path& baseDirectory, const stdfs::path& fileName, stdfs::path& out )
{
  auto currentDir = baseDirectory;
  int  depth      = 8;

  for( ;; )
  {
    auto currentFilePath = currentDir / fileName;

    auto entryInfo = fs::EntryInfo();
    auto status    = fs::getEntryInfo( currentFilePath, entryInfo );

    if( status == StatusSystemError )
      return StatusSystemError;

    if( status == StatusOk && entryInfo.type == fs::FsEntryTypeFile )
    {
      out = currentFilePath;
      return StatusOk;
    }

    currentDir = currentDir.parent_path();
    depth--;
    if( depth == 0 )
    {
      core::setErrorDetails( "project config not found: depth exceeded" );
      return StatusSystemError;
    }
  }

  return StatusNotFound;
}


Status fs::readFileJson( const char* path, Json& out )
{
  mCoreLog( "loading json at %s\n", path );

  auto bytes = std::vector<byte>();
  mCoreCheckStatus( fs::readFile( path, bytes ) );

  auto jsonString = std::string_view( reinterpret_cast<const char*>( bytes.data() ), bytes.size() );

  const bool allowExceptions = false;
  const bool ignoreComments  = true;

  out = Json::parse( jsonString, nullptr, allowExceptions, ignoreComments );
  if( out.type() == Json::value_t::discarded )
  {
    core::setErrorDetails( "can't parse json file" );
    return StatusBadFile;
  }

  return StatusOk;
}

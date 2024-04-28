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

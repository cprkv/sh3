#pragma once
#include "core/common.hpp"

namespace core::fs
{
  enum FileSeekDirection
  {
    FileSeekDirectionBegin,
    FileSeekDirectionEnd,
    FileSeekDirectionCurrent,
  };

  class File
  {
    FILE*  stream_ = nullptr;
    size_t size_   = 0;

  public:
    File( const char* path, const char* mode );
    ~File();

    size_t getSize() const { return size_; }
    bool   isOpen() const { return stream_; }
    bool   isEOF();
    size_t getPosition();

    Status seek( FileSeekDirection direction, long offset );

    Status write( const void* buffer, size_t size );
    Status read( void* buffer, size_t size );
  };


  Status        readFile( const char* path, std::vector<byte>& out );
  inline Status readFile( std::string path, std::vector<byte>& out ) { return readFile( path.c_str(), out ); }
} // namespace core::fs

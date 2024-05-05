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

  enum FsEntryType
  {
    FsEntryTypeFile,
    FsEntryTypeDirectory,
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


  struct EntryInfo
  {
    FsEntryType type;
  };


  Status        readFile( const char* path, std::vector<byte>& out );
  inline Status readFile( std::string path, std::vector<byte>& out ) { return readFile( path.c_str(), out ); }
  Status        readFileJson( const char* path, Json& out );
  inline Status readFileJson( std::string path, Json& out ) { return readFileJson( path.c_str(), out ); }
  Status        readFileMsgpack( const char* path, msgpack::object& out, msgpack::object_handle& objectHandle );
  inline Status readFileMsgpack( std::string path, msgpack::object& out, msgpack::object_handle& objectHandle ) { return readFileMsgpack( path.c_str(), out, objectHandle ); }

  Status        getEntryInfo( const stdfs::path& path, EntryInfo& out ); // returns: StatusOk / StatusSystemError / StatusNotFound
  inline Status getEntryInfo( const char* path, EntryInfo& out ) { return getEntryInfo( stdfs::path( path ), out ); }
  Status        findFileUp( const stdfs::path& baseDirectory, const stdfs::path& fileName, stdfs::path& out );

} // namespace core::fs

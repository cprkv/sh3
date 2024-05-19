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


  Status        writeFile( const char* path, std::span<const byte> data );
  inline Status writeFile( const std::string& path, std::span<const byte> data ) { return writeFile( path.c_str(), data ); }
  Status        writeFileJson( const char* path, const Json& data );
  inline Status writeFileJson( const std::string& path, const Json& data ) { return writeFileJson( path.c_str(), data ); }
  Status        readFile( const char* path, std::vector<byte>& out );
  inline Status readFile( const std::string& path, std::vector<byte>& out ) { return readFile( path.c_str(), out ); }
  Status        readFileJson( const char* path, Json& out );
  inline Status readFileJson( const std::string& path, Json& out ) { return readFileJson( path.c_str(), out ); }
  Status        readFileMsgpack( const char* path, msgpack::object& out, msgpack::object_handle& outHandle );
  inline Status readFileMsgpack( const std::string& path, msgpack::object& out, msgpack::object_handle& outHandle ) { return readFileMsgpack( path.c_str(), out, outHandle ); }

  Status        getEntryInfo( const stdfs::path& path, EntryInfo& out ); // returns: StatusOk / StatusSystemError / StatusNotFound
  inline Status getEntryInfo( const char* path, EntryInfo& out ) { return getEntryInfo( stdfs::path( path ), out ); }
  Status        findFileUp( const stdfs::path& baseDirectory, const stdfs::path& fileName, stdfs::path& out );

} // namespace core::fs

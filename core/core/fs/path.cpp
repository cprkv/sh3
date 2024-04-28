#include "core/fs/path.hpp"

using namespace core;
using namespace core::fs;


namespace
{
  std::string_view normalizePath( std::string_view path )
  {
    if( path == "/" )
      path = "";
    else
      while( path.ends_with( "/" ) )
        path = path.substr( 0, path.length() - 1 );
    return path;
  }
} // namespace


std::string fs::pathJoin( std::string_view a, std::string_view b )
{
  a = normalizePath( a );
  b = normalizePath( b );
  return std::string{ a } + "/" + std::string{ b };
}

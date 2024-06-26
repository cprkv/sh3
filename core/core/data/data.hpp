#pragma once
#include "core/common.hpp"
#include "core/data/render-chunk.hpp"
#include "core/data/serialization.hpp"

namespace core::data
{
  std::string        getDataPath( StringId id );
  std::string        getDataPath( const stdfs::path& resourceName );
  inline std::string getDataPath( const char* resourceName ) { return getDataPath( stdfs::path( resourceName ) ); }

  Status initialize();
  void   destroy();
  void   update();

} // namespace core::data

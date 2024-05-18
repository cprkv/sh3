#pragma once
#include "core/core.hpp"

namespace intermediate
{
  class FileChanges
  {
    std::string hash_;
    std::string hashPath_;
    bool        changed_;

  public:
    FileChanges( const stdfs::path& sourceFilePath,
                 const stdfs::path& destinationFilePath );

    bool isChanged() const;
    void markChanged() const;
  };

} // namespace intermediate

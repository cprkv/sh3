#pragma once
#include "core/core.hpp"
#include "schema.hpp"

namespace intermediate
{
  void processMesh( const std::string&            name,
                    const intermediate::MeshInfo& meshInfo,
                    core::data::schema::Chunk&    chunk );
}

#pragma once
#include "core/common.hpp"

namespace core::data
{
  using StringHash = u64;

  class StringId
  {
    StringHash hash_ = 0;

  public:
    constexpr StringId()                             = default;
    constexpr StringId( const StringId& )            = default;
    constexpr StringId& operator=( const StringId& ) = default;
    constexpr ~StringId() noexcept                   = default;

    constexpr StringId( StringHash hash )
        : hash_{ hash }
    {}

    StringId( const char* str );
    StringId( std::string_view string );

    constexpr StringHash getHash() const { return hash_; }

    operator StringHash() { return hash_; }

    struct Hash
    {
      size_t operator()( const StringId& s ) const noexcept;
    };

    struct EqualTo
    {
      bool operator()( const StringId& a, const StringId& b ) const noexcept;
    };

    friend constexpr auto operator<=>( const StringId&, const StringId& ) = default;
  };

  template<typename T>
  using StringIdMap = emhash8::HashMap<StringId, T, StringId::Hash, StringId::EqualTo>;

  template<typename T>
  using StringIdMultiMap = std::unordered_multimap<StringId, T, StringId::Hash, StringId::EqualTo>;
} // namespace core::data

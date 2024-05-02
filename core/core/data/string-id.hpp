#pragma once
#include "core/common.hpp"

namespace core::data
{
  using StringHash = u64;


  namespace details
  {
    constexpr StringHash stringIdHash( const char* str, size_t length )
    {
      constexpr StringHash sStringIdHashBasis = 14695981039346656037ull;
      constexpr StringHash sStringIdHashPrime = 1099511628211ull;

      StringHash hash = sStringIdHashBasis;

      for( size_t i = 0; i < length; ++i )
        hash = ( hash ^ str[i] ) * sStringIdHashPrime;

      return hash;
    }
  } // namespace details


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

    StringId( const char* str )
        : hash_{ details::stringIdHash( str, strlen( str ) ) }
    {}

    constexpr StringId( std::string_view string )
        : hash_{ details::stringIdHash( string.data(), string.length() ) }
    {}

    constexpr StringHash getHash() const { return hash_; }

    constexpr operator StringHash() { return hash_; }

    struct Hash
    {
      size_t operator()( const StringId& s ) const noexcept
      {
        return s.getHash();
      }
    };

    struct EqualTo
    {
      bool operator()( const StringId& a, const StringId& b ) const noexcept
      {
        return a.getHash() == b.getHash();
      }
    };

    friend constexpr auto operator<=>( const StringId&, const StringId& ) = default;
  };


  template<typename T>
  using StringIdMap = emhash8::HashMap<StringId, T, StringId::Hash, StringId::EqualTo>;

  template<typename T>
  using StringIdMultiMap = std::unordered_multimap<StringId, T, StringId::Hash, StringId::EqualTo>;

} // namespace core::data


constexpr core::data::StringId operator""_sid( const char* str, size_t size )
{
  return { std::string_view( str, size ) };
}

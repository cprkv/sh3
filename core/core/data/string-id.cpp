#include "core/data/string-id.hpp"

using namespace core::data;

namespace
{
  StringHash stringIdHash( const char* str, size_t length )
  {
    constexpr StringHash sStringIdHashBasis = 14695981039346656037ull;
    constexpr StringHash sStringIdHashPrime = 1099511628211ull;

    StringHash hash = sStringIdHashBasis;

    for( size_t i = 0; i < length; ++i )
      hash = ( hash ^ str[i] ) * sStringIdHashPrime;

    return hash;
  }
} // namespace


StringId::StringId( const char* str )
    : hash_{ stringIdHash( str, strlen( str ) ) }
{}

StringId::StringId( std::string_view string )
    : hash_{ stringIdHash( string.data(), string.length() ) }
{}

size_t StringId::Hash::operator()( const StringId& s ) const noexcept
{
  return s.getHash();
}

bool StringId::EqualTo::operator()( const StringId& a, const StringId& b ) const noexcept
{
  return a.getHash() == b.getHash();
}

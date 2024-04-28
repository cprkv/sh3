#include "core/utils.hpp"


std::optional<std::string> convertWideStringToMultiByte( std::wstring_view input )
{
  int length = WideCharToMultiByte( CP_ACP, 0, input.data(), ( int ) input.size(), nullptr, 0, nullptr, nullptr );
  if( !length ) return std::nullopt;

  auto str = std::string( length, ' ' );
  length   = WideCharToMultiByte( CP_ACP, 0, input.data(), ( int ) input.size(), str.data(), ( int ) str.size(), nullptr, nullptr );
  if( !length ) return std::nullopt;

  return str;
}

#include "core/utils.hpp"


std::optional<std::string> convertWideStringToMultiByte( std::wstring_view input )
{
  int length = WideCharToMultiByte( CP_ACP, 0,
                                    input.data(), static_cast<int>( input.size() ),
                                    nullptr, 0,
                                    nullptr, nullptr );
  if( !length ) return std::nullopt;

  auto str = std::string( static_cast<size_t>( length ), ' ' );
  length   = WideCharToMultiByte( CP_ACP, 0,
                                  input.data(), static_cast<int>( input.size() ),
                                  str.data(), static_cast<int>( str.size() ),
                                  nullptr, nullptr );
  if( !length ) return std::nullopt;

  return str;
}

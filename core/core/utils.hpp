#pragma once
#include "core/common.hpp"

namespace core::utils
{
  // TODO: this should go to core/system/utils?
  std::optional<std::string> convertWideStringToMultiByte( std::wstring_view input );

  template<typename R, typename... Args>
  [[maybe_unused]] auto argsTuple( R ( * )( Args... ) ) -> std::tuple<Args...>;

  template<typename F, typename... Args>
  [[maybe_unused]] auto turnIntoExpected( F&& f, Args&&... args )
  {
    using ArgT   = decltype( argsTuple( f ) );
    using Result = std::remove_reference_t<std::tuple_element_t<std::tuple_size_v<ArgT> - 1, ArgT>>;

    auto   result = Result();
    Status status = f( std::forward<Args>( args )..., result );
    if( status != StatusOk )
      return std::expected<Result, Status>( std::unexpected( status ) );
    return std::expected<Result, Status>( std::move( result ) );
  }
} // namespace core::utils

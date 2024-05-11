#pragma once
#include "core/common.hpp"
#include "core/system/message-queue.hpp"

namespace core::system::task
{
  Status init();
  void   destroy();
  void   update();

  enum PeriodicalStatus
  {
    PeriodicalStatusContinue,
    PeriodicalStatusStop,
  };

  using Task           = std::move_only_function<void()>;
  using PeriodicalTask = std::move_only_function<PeriodicalStatus()>;

  void runDeffered( Task task );
  void runAsync( Task task );
  void runPeriodical( PeriodicalTask task );


  template<typename F>
  auto ctiAsync( F&& f ) -> cti::continuable<typename std::invoke_result_t<F>::value_type>
  {
    using TResult = typename std::invoke_result_t<F>::value_type;
    return cti::make_continuable<TResult>( [f = std::move( f )]( auto&& promise ) {
      core::system::task::runAsync( [f       = std::move( f ),
                                     promise = std::forward<decltype( promise )>( promise )]() mutable {
        auto expected = f();
        if( expected.has_value() )
          promise.set_value( std::move( expected ).value() );
        else
          promise.set_exception( expected.error() );
      } );
    } );
  }


  template<typename F>
  auto ctiDeffered( F&& f ) -> cti::continuable<typename std::invoke_result_t<F>::value_type>
  {
    using TResult = typename std::invoke_result_t<F>::value_type;
    return cti::make_continuable<TResult>( [f = std::move( f )]( auto&& promise ) {
      core::system::task::runDeffered( [f       = std::move( f ),
                                        promise = std::forward<decltype( promise )>( promise )]() mutable {
        auto expected = f();
        if( expected.has_value() )
          promise.set_value( std::move( expected ).value() );
        else
          promise.set_exception( expected.error() );
      } );
    } );
  }
} // namespace core::system::task

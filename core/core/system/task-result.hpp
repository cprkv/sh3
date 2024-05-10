#pragma once
#include "core/common.hpp"

namespace core::system::task
{
  template<typename TValue, typename TState>
  class Result
  {
    struct Data
    {
      TValue result;
      TState state;
    };

    std::shared_ptr<Data> data_ = std::make_shared<Data>();

  public:
    void setResult( TValue result )
    {
      data_->result = std::move( result );
      data_->state.set();
    }

    bool isReady() const { return data_->state.isSet(); }

    TValue& value()
    {
      assert( isReady() );
      return data_->result;
    }

    const TValue& value() const
    {
      assert( isReady() );
      return data_->result;
    }

    TValue*       operator->() { return &value(); }
    const TValue* operator->() const { return &value(); }
  };


  class AsyncState
  {
    std::atomic<bool> value_ = false;

  public:
    bool isSet() const { return value_.load( std::memory_order_relaxed ); }
    void set() { value_ = true; }
  };

  class DefferedState
  {
    bool value_ = false;

  public:
    bool isSet() const { return value_; }
    void set() { value_ = true; }
  };


  template<typename TValue>
  using AsyncResult = Result<TValue, AsyncState>;

  template<typename TValue>
  using DefferedResult = Result<TValue, DefferedState>;
} // namespace core::system::task

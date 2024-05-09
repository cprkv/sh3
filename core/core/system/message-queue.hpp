#pragma once
#include "core/common.hpp"

namespace core::system
{
  template<typename T>
  class MessageQueue
  {
    std::mutex    mutex_;
    std::queue<T> container_;

  public:
    MessageQueue() = default;

    void push( T value )
    {
      auto lock = std::lock_guard( mutex_ );
      container_.emplace( std::move( value ) );
    }

    std::optional<T> tryPop()
    {
      auto lock = std::lock_guard( mutex_ );
      if( container_.empty() )
        return std::nullopt;
      auto v = std::move( container_.front() );
      container_.pop();
      return { std::move( v ) };
    }
  };

  template<typename T>
  class MessageStack
  {
    std::mutex    mutex_;
    std::stack<T> container_;

  public:
    MessageStack() = default;

    void push( T value )
    {
      auto lock = std::lock_guard( mutex_ );
      container_.emplace( std::move( value ) );
    }

    std::optional<T> tryPop()
    {
      auto lock = std::lock_guard( mutex_ );
      if( container_.empty() )
        return std::nullopt;
      auto v = std::move( container_.top() );
      container_.pop();
      return { std::move( v ) };
    }
  };
} // namespace core::system

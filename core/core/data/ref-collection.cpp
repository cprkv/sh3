#include "core/data/ref-collection.hpp"

using namespace core;
using namespace core::data;

RefCounter::RefCounter()
    : mainRef_{ nullptr }
{}

RefCounter::RefCounter( RefMain* mainRef )
    : mainRef_{ mainRef }
{
  increase();
}

RefCounter::~RefCounter()
{
  destroy();
}

RefCounter::RefCounter( const RefCounter& o ) noexcept
{
  assign( o );
  increase();
}

RefCounter& RefCounter::operator=( const RefCounter& o ) noexcept
{
  if( this != &o )
  {
    assign( o );
    increase();
  }
  return *this;
}

RefCounter::RefCounter( RefCounter&& o ) noexcept
{
  assign( o );
  o.reset();
}

RefCounter& RefCounter::operator=( RefCounter&& o ) noexcept
{
  if( this != &o )
  {
    destroy();
    assign( o );
    o.reset();
  }
  return *this;
}

void RefCounter::destroy()
{
  decrease();
}

void RefCounter::assign( const RefCounter& o )
{
  mainRef_ = o.mainRef_;
}

void RefCounter::reset()
{
  mainRef_ = nullptr;
}

void RefCounter::increase()
{
  if( !empty() )
  {
    assert( mainRef_->count < std::numeric_limits<decltype( mainRef_->count )>::max() );
    ++mainRef_->count;
  }
}

void RefCounter::decrease()
{
  if( !empty() )
  {
    assert( mainRef_->count );
    --mainRef_->count;
  }
}

#pragma once
#include "core/common.hpp"

namespace core::data
{
  struct RefMain
  {
    u32 count = 0;
  };

  class RefCounter
  {
    RefMain* mainRef_;

  public:
    RefCounter();
    explicit RefCounter( RefMain* mainRef );
    RefCounter( const RefCounter& o ) noexcept;
    RefCounter( RefCounter&& o ) noexcept;
    ~RefCounter();

    RefCounter& operator=( const RefCounter& o ) noexcept;
    RefCounter& operator=( RefCounter&& o ) noexcept;

    u32  count() const { return empty() ? 0 : mainRef_->count; }
    bool empty() const { return !mainRef_; }

  private:
    void destroy();
    void assign( const RefCounter& o );
    void reset();
    void increase();
    void decrease();
  };


  template<typename TIterator, typename TAdaptor>
  class IteratorAdapter
  {
    TIterator iterator_;

  public:
    IteratorAdapter( TIterator iterator )
        : iterator_( std::move( iterator ) )
    {}

    auto* get() noexcept { return &TAdaptor::getRef( iterator_ ); }
    auto* operator->() noexcept { return &TAdaptor::getRef( iterator_ ); }
    auto& operator*() noexcept { return TAdaptor::getRef( iterator_ ); }

    constexpr auto operator<=>( const IteratorAdapter& right ) const = default;

    IteratorAdapter& operator++()
    {
      ++iterator_;
      return *this;
    }

    IteratorAdapter operator++( int ) &
    {
      auto copy = *this;
      ++iterator_;
      return copy;
    }
  };


  template<typename TItem>
  class SimpleRefCollection
  {
    struct RefOrigin
    {
      TItem   item;
      RefMain mainRef;
    };

    using Collection = std::list<RefOrigin>;

    struct RefOriginAdapter
    {
      static TItem&       getRef( Collection::iterator it ) { return it->item; }
      static const TItem& getRef( Collection::const_iterator it ) { return it->item; }
    };

    Collection collection_;

  public:
    using Iterator      = IteratorAdapter<typename Collection::iterator, RefOriginAdapter>;
    using ConstIterator = IteratorAdapter<typename Collection::const_iterator, RefOriginAdapter>;

    SimpleRefCollection()                                            = default;
    SimpleRefCollection( SimpleRefCollection&& ) noexcept            = default;
    SimpleRefCollection& operator=( SimpleRefCollection&& ) noexcept = default;

    Iterator      begin() { return Iterator( collection_.begin() ); }
    Iterator      end() { return Iterator( collection_.end() ); }
    ConstIterator cbegin() { return ConstIterator( collection_.begin() ); }
    ConstIterator cend() { return ConstIterator( collection_.end() ); }

    RefCounter add( TItem item )
    {
      auto refOrigin = RefOrigin{ .item = std::move( item ) };
      auto iterator  = collection_.insert( collection_.end(), std::move( refOrigin ) );
      return RefCounter( &iterator->mainRef );
    }

    RefCounter getRef( TItem& item ) const
    {
      auto& origin = reinterpret_cast<RefOrigin&>( item );
      return RefCounter( &origin.mainRef );
    }

    void cleanup( std::function<void( TItem& )> onRemove )
    {
      for( auto it = collection_.begin(); it != collection_.end(); )
      {
        auto cur = it++;
        if( cur != collection_.end() &&
            cur->mainRef.count == 0 )
        {
          onRemove( cur->item );
          collection_.erase( cur );
        }
      }
    }

    void cleanup()
    {
      cleanup( []( auto& ) {} );
    }
  };
} // namespace core::data

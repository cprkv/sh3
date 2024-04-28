#pragma once

// ???
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define SDL_MAIN_HANDLED

#pragma warning( push )
#pragma warning( disable : 4702 )
#include "core/deps/hash_table8.hpp"
#include "core/deps/BS_thread_pool.hpp"
#include <msgpack.hpp>
#include <nlohmann/json.hpp>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <D3D11.h>
#include <windows.h>
#include <wrl.h>
#pragma warning( pop )

#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <span>
#include <type_traits>
#include <thread>
#include <mutex>
#include <queue>
#include <stack>
#include <variant>
#include <chrono>
#include <filesystem>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <cstdlib>
#include <cassert>
#include <cstdarg>

#define mCoreLog( ... ) fprintf( stdout, "[core] " __VA_ARGS__ )
#define mCoreLogError( ... ) fprintf( stderr, "[core][error] " __VA_ARGS__ )

#define mCoreCheckStatus( ... )                          \
  if( auto s = __VA_ARGS__; s != StatusOk ) [[unlikely]] \
    return s;

#define mFmtS "%.*s"
#define mFmtSValue( a ) static_cast<int>( ( a ).size() ), ( a ).data()
#define mFmtSize "%zu"
#define mFmtU32 "%" PRIu32
#define mFmtU64 "%" PRIu64
#define mFmtS32 "%" PRId32
#define mFmtS64 "%" PRId64
#define mFmtF32 "%f"

#define mFmtVec3 "(%02.3f %02.3f %02.3f)"
#define mFmtVec3Value( a ) a.x, a.y, a.z

#define mFmtMat4 "(%02.3f %02.3f %02.3f %02.3f\n" \
                 " %02.3f %02.3f %02.3f %02.3f\n" \
                 " %02.3f %02.3f %02.3f %02.3f\n" \
                 " %02.3f %02.3f %02.3f %02.3f)"
#define mFmtMat4Value( a ) reinterpret_cast<const float*>( &( a ) )[0], reinterpret_cast<const float*>( &( a ) )[1], reinterpret_cast<const float*>( &( a ) )[2], reinterpret_cast<const float*>( &( a ) )[3],   \
                           reinterpret_cast<const float*>( &( a ) )[4], reinterpret_cast<const float*>( &( a ) )[5], reinterpret_cast<const float*>( &( a ) )[6], reinterpret_cast<const float*>( &( a ) )[7],   \
                           reinterpret_cast<const float*>( &( a ) )[8], reinterpret_cast<const float*>( &( a ) )[9], reinterpret_cast<const float*>( &( a ) )[10], reinterpret_cast<const float*>( &( a ) )[11], \
                           reinterpret_cast<const float*>( &( a ) )[12], reinterpret_cast<const float*>( &( a ) )[13], reinterpret_cast<const float*>( &( a ) )[14], reinterpret_cast<const float*>( &( a ) )[15]

namespace stdfs = std::filesystem;

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using s8    = int8_t;
using s16   = int16_t;
using s32   = int32_t;
using s64   = int64_t;
using f32   = float;
using f64   = double;
using usize = size_t;
using byte  = u8;
using uptr  = uintptr_t;

using Vec2  = glm::vec2;
using Vec2u = glm::uvec2;
using Vec2s = glm::ivec2;
using Vec2b = glm::u8vec2;

using Vec3  = glm::vec3;
using Vec3u = glm::uvec3;
using Vec3s = glm::ivec3;
using Vec3b = glm::u8vec3;

using Vec4  = glm::vec4;
using Vec4u = glm::uvec4;
using Vec4s = glm::ivec4;
using Vec4b = glm::u8vec4;

using Mat4 = glm::mat4;
using Mat3 = glm::mat3;

using Quat = glm::fquat;

enum Status
{
  StatusOk,
  StatusSystemError,
  StatusBadFile,
  StatusBufferOverflow,
  StatusNotFound,
};

namespace core
{
  std::string formatMessage( const char* fmt, va_list args );
  const char* getErrorDetails();
  void        setErrorDetails( const char* message, ... );
} // namespace core


class ArrayBytesView
{
  byte* data_;
  usize count_;
  usize elementSize_;

public:
  constexpr ArrayBytesView( byte* data, usize count, usize elementSize ) noexcept
      : data_( data )
      , count_( count )
      , elementSize_( elementSize )
  {}

  constexpr ArrayBytesView( const ArrayBytesView& ) noexcept            = default;
  constexpr ArrayBytesView( ArrayBytesView&& ) noexcept                 = default;
  constexpr ArrayBytesView& operator=( const ArrayBytesView& ) noexcept = default;
  constexpr ArrayBytesView& operator=( ArrayBytesView&& ) noexcept      = default;
  constexpr ~ArrayBytesView() noexcept                                  = default;

  // constexpr byte* begin() { return data_.data_; }
  // constexpr byte* end() { return data_.data_ + data_.size * elementSize_; }
  // constexpr const byte* begin() const { return data_.data_; }
  // constexpr const byte* end() const { return data_.data_ + data_.size * elementSize_; }

  constexpr byte*       getData() noexcept { return data_; }
  constexpr const byte* getData() const noexcept { return data_; }

  constexpr usize getSize() const noexcept { return count_; }
  constexpr usize getElementSize() const noexcept { return elementSize_; }

  template<typename T>
  static constexpr ArrayBytesView from( T* tdata, usize count = 1 ) noexcept
  {
    auto* data = reinterpret_cast<byte*>( tdata );
    return { data, count, sizeof( T ) };
  }

  template<typename T, size_t Count>
  static constexpr ArrayBytesView fromStatic( T ( &tdata )[Count] ) noexcept
  {
    // TODO: this is hack to clear constexpr refs. in future ArrayBytesView should accept const types and manage it as consts
    using NotConstT = std::add_lvalue_reference_t<std::remove_const_t<T>[Count]>;
    auto* data      = reinterpret_cast<byte*>( const_cast<NotConstT>( tdata ) );
    return { data, Count, sizeof( T ) };
  }

  template<typename T>
  static constexpr ArrayBytesView fromContainer( T& container ) noexcept
  {
    auto* data = reinterpret_cast<byte*>( container.data() );
    return { data, container.size(), sizeof( typename T::value_type ) };
  }
};


template<typename T>
constexpr auto makeArrayBytesView( T* data, usize count ) noexcept
{
  return ArrayBytesView::from<T>( data, count );
}


template<typename T, size_t Size>
constexpr auto makeArrayBytesView( T ( &arr )[Size] ) noexcept
{
  return ArrayBytesView::fromStatic<T>( arr );
}


template<typename T>
  requires std::is_class_v<T>
constexpr auto makeArrayBytesView( T& container ) noexcept
{
  return ArrayBytesView::fromContainer<T>( container );
}


template<typename T>
class ArrayView
{
  T*    data_;
  usize size_;

public:
  using value_type = T;

  constexpr explicit ArrayView( std::span<T> span ) noexcept
      : data_( span.data() )
      , size_( span.size() )
  {}

  constexpr ArrayView( T* data, usize size ) noexcept
      : data_( data )
      , size_( size )
  {}

  constexpr ArrayView( const ArrayView& )                = default;
  constexpr ArrayView( ArrayView&& ) noexcept            = default;
  constexpr ArrayView& operator=( const ArrayView& )     = default;
  constexpr ArrayView& operator=( ArrayView&& ) noexcept = default;
  constexpr ~ArrayView()                                 = default;

  constexpr T* begin() noexcept { return data_; }
  constexpr T* begin() const noexcept { return data_; }

  constexpr T* end() noexcept { return data_ + size_; }
  constexpr T* end() const noexcept { return data_ + size_; }

  constexpr T*       getData() noexcept { return data_; }
  constexpr const T* getData() const noexcept { return data_; }

  constexpr T&       operator[]( usize i ) noexcept { return data_[i]; }
  constexpr const T& operator[]( usize i ) const noexcept { return data_[i]; }

  constexpr usize getSize() const noexcept { return size_; }
  constexpr usize getElementSize() const noexcept { return sizeof( T ); }

  template<typename TArr>
  static constexpr ArrayView from( TArr* arr, usize size ) noexcept
  {
    return { arr, size };
  }

  template<size_t Size>
  static constexpr ArrayView fromArray( T ( &arr )[Size] ) noexcept
  {
    return { arr, Size };
  }

  template<typename TContainer>
  static constexpr ArrayView fromContainer( TContainer& container ) noexcept
  {
    return { container.data(), container.size() };
  }
};


template<typename T>
constexpr auto makeArrayView( T* array, usize size ) noexcept
{
  return ArrayView<T>::from( array, size );
}


template<typename T, size_t Size>
constexpr auto makeArrayView( T ( &arr )[Size] ) noexcept
{
  return ArrayView<T>::from_array( arr );
}


template<typename T>
constexpr auto makeArrayView( T& container ) noexcept
{
  return ArrayView<typename T::value_type>::fromContainer( container );
}


template<class T>
class ArrayConstIterator
{
public:
  using value_type      = T;
  using difference_type = ptrdiff_t;
  using pointer         = const T*;
  using reference       = const T&;

  constexpr ArrayConstIterator()
      : ptr_( nullptr )
  {}

  constexpr explicit ArrayConstIterator( pointer ptr, u32 offset = 0 )
      : ptr_( ptr + offset )
  {}

  constexpr reference operator*() const { return *ptr_; }
  constexpr pointer   operator->() const { return ptr_; }

  constexpr ArrayConstIterator& operator++()
  {
    ++ptr_;
    return *this;
  }

  constexpr ArrayConstIterator operator++( int )
  {
    ArrayConstIterator tmp = *this;
    ++ptr_;
    return tmp;
  }

  constexpr ArrayConstIterator& operator--()
  {
    --ptr_;
    return *this;
  }

  constexpr ArrayConstIterator operator--( int )
  {
    ArrayConstIterator tmp = *this;
    --ptr_;
    return tmp;
  }

  constexpr ArrayConstIterator& operator+=( const ptrdiff_t offset )
  {
    ptr_ += offset;
    return *this;
  }

  constexpr ArrayConstIterator operator+( const ptrdiff_t offset ) const
  {
    ArrayConstIterator tmp = *this;
    return tmp += offset;
  }

  constexpr ArrayConstIterator& operator-=( const ptrdiff_t offset )
  {
    ptr_ -= offset;
    return *this;
  }

  constexpr ArrayConstIterator operator-( const ptrdiff_t offset ) const
  {
    ArrayConstIterator tmp = *this;
    return tmp -= offset;
  }

  constexpr ptrdiff_t operator-( const ArrayConstIterator& right ) const
  {
    return ptr_ - right.ptr_;
  }

  constexpr reference operator[]( const ptrdiff_t offset ) const { return ptr_[offset]; }
  constexpr bool      operator==( const ArrayConstIterator& right ) const { return ptr_ == right.ptr_; }
  constexpr bool      operator!=( const ArrayConstIterator& right ) const { return !( *this == right ); }
  constexpr bool      operator<( const ArrayConstIterator& right ) const { return ptr_ < right.ptr_; }
  constexpr bool      operator>( const ArrayConstIterator& right ) const { return right < *this; }
  constexpr bool      operator<=( const ArrayConstIterator& right ) const { return !( right < *this ); }
  constexpr bool      operator>=( const ArrayConstIterator& right ) const { return !( *this < right ); }

private:
  pointer ptr_;
};

//-----------------------------------------------------------------------

template<class T>
class ArrayIterator : public ArrayConstIterator<T>
{
public:
  using base              = ArrayConstIterator<T>;
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = T;
  using difference_type   = ptrdiff_t;
  using pointer           = T*;
  using reference         = T&;

  constexpr ArrayIterator() = default;

  constexpr explicit ArrayIterator( pointer ptr, u32 offset = 0 )
      : base( ptr, offset )
  {}

  constexpr reference operator*() const
  {
    return const_cast<reference>( base::operator*() );
  }

  constexpr pointer operator->() const
  {
    return const_cast<pointer>( base::operator->() );
  }

  constexpr ArrayIterator& operator++()
  {
    base::operator++();
    return *this;
  }

  constexpr ArrayIterator operator++( int )
  {
    ArrayIterator tmp = *this;
    base::operator++();
    return tmp;
  }

  constexpr ArrayIterator& operator--()
  {
    base::operator--();
    return *this;
  }

  constexpr ArrayIterator operator--( int )
  {
    ArrayIterator tmp = *this;
    base::operator--();
    return tmp;
  }

  constexpr ArrayIterator& operator+=( const ptrdiff_t offset )
  {
    base::operator+=( offset );
    return *this;
  }

  constexpr ArrayIterator operator+( const ptrdiff_t offset ) const
  {
    ArrayIterator tmp = *this;
    return tmp += offset;
  }

  constexpr ArrayIterator& operator-=( const ptrdiff_t offset )
  {
    base::operator-=( offset );
    return *this;
  }

  using base::operator-;

  constexpr ArrayIterator operator-( const ptrdiff_t offset ) const
  {
    ArrayIterator tmp = *this;
    return tmp -= offset;
  }

  constexpr reference operator[]( const ptrdiff_t offset ) const
  {
    return const_cast<reference>( base::operator[]( offset ) );
  }
};

//-----------------------------------------------------------------------

// todo there is no move-semantic because it overcomplicate things with construction and destruction
template<class T, u32 capacity>
class StaticVector
{
  T   elems_[capacity];
  u32 size_ = 0;

public:
  using value_type             = T;
  using size_type              = u32;
  using difference_type        = ptrdiff_t;
  using pointer                = T*;
  using const_pointer          = const T*;
  using reference              = T&;
  using const_reference        = const T&;
  using iterator               = ArrayIterator<T>;
  using const_iterator         = ArrayConstIterator<T>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  StaticVector()                                 = default;
  StaticVector( const StaticVector& )            = default;
  StaticVector& operator=( const StaticVector& ) = default;

  explicit StaticVector( u32 size )
      : size_( size )
  {}

  StaticVector( std::initializer_list<T> items )
  {
    for( auto&& item: items )
    {
      push_back( std::move( item ) );
    }
  }

  StaticVector( StaticVector&& o ) noexcept( std::is_nothrow_swappable_v<T> )
      : StaticVector()
  {
    swap( o );
  }

  StaticVector& operator=( StaticVector&& o ) noexcept( std::is_nothrow_swappable_v<T> )
  {
    if( this != &o ) swap( o );
    return *this;
  }

  void resize( u32 size )
  {
    assert( size <= capacity );
    size_ = size;
  }

  void                             fill( const T& val ) noexcept { std::fill_n( elems_, size_, val ); }
  constexpr iterator               begin() noexcept { return iterator( elems_, 0 ); }
  constexpr const_iterator         begin() const noexcept { return const_iterator( elems_, 0 ); }
  constexpr iterator               end() noexcept { return iterator( elems_, size_ ); }
  constexpr const_iterator         end() const noexcept { return const_iterator( elems_, size_ ); }
  constexpr reverse_iterator       rbegin() noexcept { return reverse_iterator( end() ); }
  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator( end() ); }
  constexpr reverse_iterator       rend() noexcept { return reverse_iterator( begin() ); }
  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator( begin() ); }
  constexpr const_iterator         cbegin() const noexcept { return begin(); }
  constexpr const_iterator         cend() const noexcept { return end(); }
  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  constexpr const_reverse_iterator crend() const noexcept { return rend(); }
  constexpr size_type              size() const noexcept { return size_; }
  constexpr size_type              max_size() const noexcept { return capacity; }
  constexpr bool                   empty() const noexcept { return size_ == 0; }
  constexpr reference              at( size_type pos ) noexcept { return elems_[pos]; }
  constexpr const_reference        at( size_type pos ) const noexcept { return elems_[pos]; }
  constexpr reference              operator[]( size_type pos ) noexcept { return elems_[pos]; }
  constexpr const_reference        operator[]( size_type pos ) const noexcept { return elems_[pos]; }
  constexpr reference              front() noexcept { return elems_[0]; }
  constexpr const_reference        front() const noexcept { return elems_[0]; }
  constexpr reference              back() noexcept { return elems_[size_ - 1]; }
  constexpr const_reference        back() const noexcept { return elems_[size_ - 1]; }
  constexpr pointer                data() noexcept { return elems_; }
  constexpr const_pointer          data() const noexcept { return elems_; }
  constexpr void                   clear() noexcept { size_ = 0; }
  constexpr ArrayView<T>           view() noexcept { return make_array_view( &elems_, size_ ); }

  constexpr reference push_back( const T& val ) noexcept
  {
    assert( size_ != capacity );
    return ( elems_[size_++] = val );
  }

  constexpr reference push_back( T& val ) noexcept
  {
    assert( size_ != capacity );
    return ( elems_[size_++] = val );
  }

  constexpr reference emplace_back( T&& val ) noexcept
  {
    assert( size_ != capacity );
    return ( elems_[size_++] = std::move( val ) );
  }

  constexpr T pop_back() noexcept
  {
    assert( size_ != 0 );
    return elems_[--size_];
  }

  void swap( StaticVector& right ) noexcept( std::is_nothrow_swappable_v<T> )
  {
    std::swap_ranges( elems_, elems_ + capacity, right.elems_ );
    std::swap( size_, right.size_ );
  }
};

template<class... T>
StaticVector( T&&... t ) -> StaticVector<std::common_type_t<T...>, static_cast<u32>( sizeof...( T ) )>;

#include "schema.hpp"

using namespace intermediate;

FileWriter::FileWriter( const char* path )
    : file( path, "wb" )
{
  hasError = !file.isOpen();
}

void FileWriter::write( const char* data, size_t size )
{
  if( hasError )
    return;

  auto s = file.write( data, size );
  if( s != StatusOk )
    hasError = true;
}


const msgpack::object& msgpack::adaptor::convert<Vec2s>::operator()(
    const msgpack::object& o, Vec2s& v ) const
{
  if( o.type != msgpack::type::ARRAY ) throw msgpack::type_error();
  if( o.via.array.size != 2 ) throw msgpack::type_error();
  v = Vec2s( o.via.array.ptr[0].as<int>(),
             o.via.array.ptr[1].as<int>() );
  return o;
}

msgpack::packer<FileWriter>& msgpack::adaptor::pack<Vec2s>::operator()(
    msgpack::packer<FileWriter>& o, const Vec2s& v ) const
{
  o.pack_array( 2 );
  o.pack( v.x );
  o.pack( v.y );
  return o;
}


const msgpack::object& msgpack::adaptor::convert<Vec3>::operator()(
    const msgpack::object& o, Vec3& v ) const
{
  if( o.type != msgpack::type::ARRAY ) throw msgpack::type_error();
  if( o.via.array.size != 3 ) throw msgpack::type_error();
  v = Vec3( o.via.array.ptr[0].as<float>(),
            o.via.array.ptr[1].as<float>(),
            o.via.array.ptr[2].as<float>() );
  return o;
}

msgpack::packer<intermediate::FileWriter>& msgpack::adaptor::pack<Vec3>::operator()(
    msgpack::packer<intermediate::FileWriter>& o, const Vec3& v ) const
{
  o.pack_array( 3 );
  o.pack( v.x );
  o.pack( v.y );
  o.pack( v.z );
  return o;
}


const msgpack::object& msgpack::adaptor::convert<Vec4>::operator()(
    const msgpack::object& o, Vec4& v ) const
{
  if( o.type != msgpack::type::ARRAY ) throw msgpack::type_error();
  if( o.via.array.size != 4 ) throw msgpack::type_error();
  v = Vec4( o.via.array.ptr[0].as<float>(),
            o.via.array.ptr[1].as<float>(),
            o.via.array.ptr[2].as<float>(),
            o.via.array.ptr[3].as<float>() );
  return o;
}

msgpack::packer<intermediate::FileWriter>& msgpack::adaptor::pack<Vec4>::operator()(
    msgpack::packer<intermediate::FileWriter>& o, const Vec4& v ) const
{
  o.pack_array( 4 );
  o.pack( v.x );
  o.pack( v.y );
  o.pack( v.z );
  o.pack( v.w );
  return o;
}


const msgpack::object& msgpack::adaptor::convert<Quat>::operator()(
    const msgpack::object& o, Quat& v ) const
{
  if( o.type != msgpack::type::ARRAY ) throw msgpack::type_error();
  if( o.via.array.size != 4 ) throw msgpack::type_error();
  // packed as x,y,z,w
  v = Quat( o.via.array.ptr[3].as<float>(),
            o.via.array.ptr[0].as<float>(),
            o.via.array.ptr[1].as<float>(),
            o.via.array.ptr[2].as<float>() );
  return o;
}

msgpack::packer<intermediate::FileWriter>& msgpack::adaptor::pack<Quat>::operator()(
    msgpack::packer<intermediate::FileWriter>& o, const Quat& v ) const
{
  o.pack_array( 4 );
  o.pack( v.x );
  o.pack( v.y );
  o.pack( v.z );
  o.pack( v.w );
  return o;
}

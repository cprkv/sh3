#include "unit_test_framework/unit_test_framework.hpp"
#include "core/math/math.hpp"


TEST( math_color )
{
  auto decoded = core::math::decodeColorHex( 0xE772FF );
  ASSERT_EQUAL( decoded.x, 231.f / 255.f );
  ASSERT_EQUAL( decoded.y, 114.f / 255.f );
  ASSERT_EQUAL( decoded.z, 255.f / 255.f );
}

TEST( math_bb )
{
  auto bb   = core::math::BoundingBox();
  bb.center = Vec3( 1, 1, 1 );
  bb.bx     = Vec3( 1, 0, 0 );
  bb.by     = Vec3( 0, 1, 0 );
  bb.bz     = Vec3( 0, 0, 1 );

  ASSERT_TRUE( bb.isInside( Vec3( 1.5f, 1.5f, 1.5f ) ) );
  ASSERT_FALSE( bb.isInside( Vec3( 0.5f, 1.5f, 1.5f ) ) );
  ASSERT_FALSE( bb.isInside( Vec3( 1.5f, 0.5f, 1.5f ) ) );
  ASSERT_FALSE( bb.isInside( Vec3( 1.5f, 1.5f, 0.5f ) ) );
  ASSERT_FALSE( bb.isInside( Vec3( 0.5f, 0.5f, 0.5f ) ) );
  ASSERT_FALSE( bb.isInside( Vec3( 2.5f, 2.5f, 2.5f ) ) );
}

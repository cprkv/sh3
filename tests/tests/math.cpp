#include "unit_test_framework/unit_test_framework.hpp"
#include "core/math/math.hpp"


TEST( math_color )
{
  auto decoded = core::math::decodeColorHex( 0xE772FF );
  ASSERT_EQUAL( decoded.x, 231.f / 255.f );
  ASSERT_EQUAL( decoded.y, 114.f / 255.f );
  ASSERT_EQUAL( decoded.z, 255.f / 255.f );
}

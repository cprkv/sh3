#include "unit_test_framework/unit_test_framework.hpp"
#include "core/math/math.hpp"

TEST( math_Mat4 )
{
  {
    auto m = math::Mat4();
    auto v = m * math::Vec4f( 1, 2, 3 );
    ASSERT_EQUAL( v, math::Vec4f( 1, 2, 3 ) );
  }

  // translation
  {
    auto m = math::Mat4::translation( math::Vec3f( 1, 2, 3 ) );
    auto v = m * math::Vec4f( 1, 2, 3 ); // dir vector
    ASSERT_EQUAL( v, math::Vec4f( 1, 2, 3 ) );
  }
  {
    auto m = math::Mat4::translation( math::Vec3f( 1, 2, 3 ) );
    auto v = m * math::Vec4f( 1, 2, 3, 1 ); // pos vector
    ASSERT_EQUAL( v, math::Vec4f( 2, 4, 6, 1 ) );
  }

  // scale
  {
    auto m = math::Mat4::scale( math::Vec3f( 1, 2, 3 ) );
    auto v = m * math::Vec4f( 1, 1, 2 ); // dir vector
    ASSERT_EQUAL( v, math::Vec4f( 1, 2, 6 ) );
  }
  {
    auto m = math::Mat4::scale( math::Vec3f( 1, 2, 3 ) );
    auto v = m * math::Vec4f( 1, 1, 2, 1 ); // pos vector
    ASSERT_EQUAL( v, math::Vec4f( 1, 2, 6, 1 ) );
  }
}

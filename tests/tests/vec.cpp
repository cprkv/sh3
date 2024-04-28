#include "unit_test_framework/unit_test_framework.hpp"
#include "core/math/math.hpp"

TEST( math_Vec2_init )
{
  {
    math::Vec2f vec;
    ASSERT_EQUAL( vec.x, 0.f );
    ASSERT_EQUAL( vec.y, 0.f );
  }

  {
    math::Vec2f vec( 1 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 1.f );
  }

  {
    math::Vec2f vec( 1, 2 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
  }

  {
    math::Vec2f a( 1, 2 );
    math::Vec2f vec( a );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
  }

  {
    math::Vec2f a( 1, 2 );
    math::Vec2f vec;
    vec = a;
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
  }

  {
    auto vec = math::Vec2f::from( math::Vec2s( 1, 2 ) );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
  }


  {
    math::Vec2s vec;
    ASSERT_EQUAL( vec.x, 0 );
    ASSERT_EQUAL( vec.y, 0 );
  }

  {
    math::Vec2s vec( 1 );
    ASSERT_EQUAL( vec.x, 1 );
    ASSERT_EQUAL( vec.y, 1 );
  }

  {
    math::Vec2s vec( 1, 2 );
    ASSERT_EQUAL( vec.x, 1 );
    ASSERT_EQUAL( vec.y, 2 );
  }

  {
    math::Vec2s a( 1, 2 );
    math::Vec2s vec( a );
    ASSERT_EQUAL( vec.x, 1 );
    ASSERT_EQUAL( vec.y, 2 );
  }

  {
    math::Vec2s a( 1, 2 );
    math::Vec2s vec;
    vec = a;
    ASSERT_EQUAL( vec.x, 1 );
    ASSERT_EQUAL( vec.y, 2 );
  }

  {
    auto vec = math::Vec2s::from( math::Vec2f( 1.1f, 2.2f ) );
    ASSERT_EQUAL( vec.x, 1 );
    ASSERT_EQUAL( vec.y, 2 );
  }
}

TEST( math_Vec3_init )
{
  {
    math::Vec3f vec;
    ASSERT_EQUAL( vec.x, 0.f );
    ASSERT_EQUAL( vec.y, 0.f );
    ASSERT_EQUAL( vec.z, 0.f );
  }

  {
    math::Vec3f vec( 1 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 1.f );
    ASSERT_EQUAL( vec.z, 1.f );
  }

  {
    math::Vec3f vec( 1, 2 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 0.f );
  }

  {
    math::Vec3f vec( 1, 2, 3 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
  }

  {
    math::Vec3f a( 1, 2, 3 );
    math::Vec3f vec( a );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
  }

  {
    math::Vec3f a( 1, 2, 3 );
    math::Vec3f vec;
    vec = a;
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
  }

  {
    auto vec = math::Vec3f::from( math::Vec3s( 1, 2, 3 ) );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
  }
}

TEST( math_Vec4_init )
{
  {
    math::Vec4f vec;
    ASSERT_EQUAL( vec.x, 0.f );
    ASSERT_EQUAL( vec.y, 0.f );
    ASSERT_EQUAL( vec.z, 0.f );
    ASSERT_EQUAL( vec.w, 0.f );
  }

  {
    math::Vec4f vec( 1 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 1.f );
    ASSERT_EQUAL( vec.z, 1.f );
    ASSERT_EQUAL( vec.w, 1.f );
  }

  {
    math::Vec4f vec( 1, 2 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 0.f );
    ASSERT_EQUAL( vec.w, 0.f );
  }

  {
    math::Vec4f vec( 1, 2, 3 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
    ASSERT_EQUAL( vec.w, 0.f );
  }

  {
    math::Vec4f vec( 1, 2, 3, 4 );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
    ASSERT_EQUAL( vec.w, 4.f );
  }

  {
    math::Vec4f a( 1, 2, 3, 4 );
    math::Vec4f vec( a );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
    ASSERT_EQUAL( vec.w, 4.f );
  }

  {
    math::Vec4f a( 1, 2, 3, 4 );
    math::Vec4f vec;
    vec = a;
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
    ASSERT_EQUAL( vec.w, 4.f );
  }

  {
    auto vec = math::Vec4f::from( math::Vec4s( 1, 2, 3, 4 ) );
    ASSERT_EQUAL( vec.x, 1.f );
    ASSERT_EQUAL( vec.y, 2.f );
    ASSERT_EQUAL( vec.z, 3.f );
    ASSERT_EQUAL( vec.w, 4.f );
  }
}

//------------------------------------------------------------------------------------------------------------------

TEST( math_Vec_length )
{
  ASSERT_EQUAL( math::Vec2f( 2, 3 ).sqr(), 13.f );
  ASSERT_EQUAL( math::Vec3f( 2, 3, 4 ).sqr(), 29.f );
  ASSERT_EQUAL( math::Vec4f( 2, 3, 4, 5 ).sqr(), 54.f );

  ASSERT_EQUAL( math::Vec2f( 3, 4 ).length(), 5.f );
  ASSERT_EQUAL( math::Vec3f( 4, 12, 3 ).length(), 13.f );
  ASSERT_EQUAL( math::Vec4f( 10, 14, 8, 1 ).length(), 19.f );


  ASSERT_EQUAL( math::Vec2f( 5, 0 ).normalized(), math::Vec2f( 1, 0 ) );
  ASSERT_EQUAL( math::Vec2f( 0, 5 ).normalized(), math::Vec2f( 0, 1 ) );
  ASSERT_EQUAL( math::Vec2f( 0 ).normalized(), math::Vec2f( 0, 0 ) );

  ASSERT_EQUAL( math::Vec3f( 5, 0, 0 ).normalized(), math::Vec3f( 1, 0, 0 ) );
  ASSERT_EQUAL( math::Vec3f( 0, 5, 0 ).normalized(), math::Vec3f( 0, 1, 0 ) );
  ASSERT_EQUAL( math::Vec3f( 0, 0, 5 ).normalized(), math::Vec3f( 0, 0, 1 ) );
  ASSERT_EQUAL( math::Vec3f( 0 ).normalized(), math::Vec3f( 0 ) );

  ASSERT_EQUAL( math::Vec4f( 5, 0, 0, 0 ).normalized(), math::Vec4f( 1, 0, 0, 0 ) );
  ASSERT_EQUAL( math::Vec4f( 0, 5, 0, 0 ).normalized(), math::Vec4f( 0, 1, 0, 0 ) );
  ASSERT_EQUAL( math::Vec4f( 0, 0, 5, 0 ).normalized(), math::Vec4f( 0, 0, 1, 0 ) );
  ASSERT_EQUAL( math::Vec4f( 0, 0, 0, -5 ).normalized(), math::Vec4f( 0, 0, 0, -1 ) );
  ASSERT_EQUAL( math::Vec4f( 0 ).normalized(), math::Vec4f( 0 ) );
}


TEST( math_Vec_eq )
{
  ASSERT_EQUAL( math::Vec2f( 2, 3 ), math::Vec2f( 2, 3 ) );
  ASSERT_EQUAL( math::Vec3f( 2, 3, 4 ), math::Vec3f( 2, 3, 4 ) );
  ASSERT_EQUAL( math::Vec4f( 2, 3, 4, 5 ), math::Vec4f( 2, 3, 4, 5 ) );

  ASSERT_NOT_EQUAL( math::Vec2f( 2, 3 ), math::Vec2f( 2, 3.3f ) );
  ASSERT_NOT_EQUAL( math::Vec3f( 2, 3, 4 ), math::Vec3f( 2, 3.3f, 4 ) );
  ASSERT_NOT_EQUAL( math::Vec4f( 2, 3, 4, 5 ), math::Vec4f( 2, 3.3f, 4, 5 ) );


  ASSERT_EQUAL( math::Vec2f(), math::Vec2f::gZero );
  ASSERT_EQUAL( math::Vec3f(), math::Vec3f::gZero );
  ASSERT_EQUAL( math::Vec4f(), math::Vec4f::gZero );

  ASSERT_EQUAL( math::Vec2u(), math::Vec2u::gZero );
  ASSERT_EQUAL( math::Vec3u(), math::Vec3u::gZero );
  ASSERT_EQUAL( math::Vec4u(), math::Vec4u::gZero );

  ASSERT_EQUAL( math::Vec2s(), math::Vec2s::gZero );
  ASSERT_EQUAL( math::Vec3s(), math::Vec3s::gZero );
  ASSERT_EQUAL( math::Vec4s(), math::Vec4s::gZero );
}


TEST( math_Vec2_operators )
{
  {
    auto a = math::Vec2f( 2, 3 );
    ASSERT_EQUAL( -a, math::Vec2f( -2, -3 ) );
    ASSERT_EQUAL( a + math::Vec2f( 1, 2 ), math::Vec2f( 3, 5 ) );
    ASSERT_EQUAL( a - math::Vec2f( 1, 2 ), math::Vec2f( 1, 1 ) );
    ASSERT_EQUAL( a * math::Vec2f( 1, 2 ), math::Vec2f( 2, 6 ) );
    ASSERT_EQUAL( a / math::Vec2f( 1, 2 ), math::Vec2f( 2, 1.5f ) );
  }

  {
    auto a = math::Vec2f( 2, 3 );
    a += math::Vec2f( 1, 2 );
    ASSERT_EQUAL( a, math::Vec2f( 3, 5 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a -= math::Vec2f( 1, 2 );
    ASSERT_EQUAL( a, math::Vec2f( 1, 1 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a *= math::Vec2f( 1, 2 );
    ASSERT_EQUAL( a, math::Vec2f( 2, 6 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a /= math::Vec2f( 1, 2 );
    ASSERT_EQUAL( a, math::Vec2f( 2, 1.5f ) );
  }


  {
    auto a = math::Vec2f( 2, 3 );
    ASSERT_EQUAL( a + 2, math::Vec2f( 4, 5 ) );
    ASSERT_EQUAL( a - 2, math::Vec2f( 0, 1 ) );
    ASSERT_EQUAL( a * 2, math::Vec2f( 4, 6 ) );
    ASSERT_EQUAL( a / 2, math::Vec2f( 1, 1.5f ) );
  }

  {
    auto a = math::Vec2f( 2, 3 );
    a += 2;
    ASSERT_EQUAL( a, math::Vec2f( 4, 5 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a -= 2;
    ASSERT_EQUAL( a, math::Vec2f( 0, 1 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a *= 2;
    ASSERT_EQUAL( a, math::Vec2f( 4, 6 ) );
  }
  {
    auto a = math::Vec2f( 2, 3 );
    a /= 2;
    ASSERT_EQUAL( a, math::Vec2f( 1, 1.5f ) );
  }
}


TEST( math_Vec3_operators )
{
  {
    auto a = math::Vec3f( 2, 3, 4 );
    ASSERT_EQUAL( -a, math::Vec3f( -2, -3, -4 ) );
    ASSERT_EQUAL( a + math::Vec3f( 1, 2, 3 ), math::Vec3f( 3, 5, 7 ) );
    ASSERT_EQUAL( a - math::Vec3f( 1, 2, 3 ), math::Vec3f( 1, 1, 1 ) );
    ASSERT_EQUAL( a * math::Vec3f( 1, 2, 3 ), math::Vec3f( 2, 6, 12 ) );
    ASSERT_EQUAL( a / math::Vec3f( 1, 2, 3 ), math::Vec3f( 2, 1.5f, 4.f / 3.f ) );
  }

  {
    auto a = math::Vec3f( 2, 3, 4 );
    a += math::Vec3f( 1, 2, 3 );
    ASSERT_EQUAL( a, math::Vec3f( 3, 5, 7 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a -= math::Vec3f( 1, 2, 3 );
    ASSERT_EQUAL( a, math::Vec3f( 1, 1, 1 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a *= math::Vec3f( 1, 2, 3 );
    ASSERT_EQUAL( a, math::Vec3f( 2, 6, 12 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a /= math::Vec3f( 1, 2, 3 );
    ASSERT_EQUAL( a, math::Vec3f( 2, 1.5f, 4.f / 3.f ) );
  }


  {
    auto a = math::Vec3f( 2, 3, 4 );
    ASSERT_EQUAL( a + 2, math::Vec3f( 4, 5, 6 ) );
    ASSERT_EQUAL( a - 2, math::Vec3f( 0, 1, 2 ) );
    ASSERT_EQUAL( a * 2, math::Vec3f( 4, 6, 8 ) );
    ASSERT_EQUAL( a / 2, math::Vec3f( 1, 1.5f, 2 ) );
  }

  {
    auto a = math::Vec3f( 2, 3, 4 );
    a += 2;
    ASSERT_EQUAL( a, math::Vec3f( 4, 5, 6 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a -= 2;
    ASSERT_EQUAL( a, math::Vec3f( 0, 1, 2 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a *= 2;
    ASSERT_EQUAL( a, math::Vec3f( 4, 6, 8 ) );
  }
  {
    auto a = math::Vec3f( 2, 3, 4 );
    a /= 2;
    ASSERT_EQUAL( a, math::Vec3f( 1, 1.5f, 2 ) );
  }
}


TEST( math_Vec4_operators )
{
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    ASSERT_EQUAL( -a, math::Vec4f( -2, -3, -4, -5 ) );
    ASSERT_EQUAL( a + math::Vec4f( 1, 2, 3, 4 ), math::Vec4f( 3, 5, 7, 9 ) );
    ASSERT_EQUAL( a - math::Vec4f( 1, 2, 3, 4 ), math::Vec4f( 1, 1, 1, 1 ) );
    ASSERT_EQUAL( a * math::Vec4f( 1, 2, 3, 4 ), math::Vec4f( 2, 6, 12, 20 ) );
    ASSERT_EQUAL( a / math::Vec4f( 1, 2, 3, 4 ), math::Vec4f( 2, 1.5f, 4.f / 3.f, 5.f / 4.f ) );
  }

  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a += math::Vec4f( 1, 2, 3, 4 );
    ASSERT_EQUAL( a, math::Vec4f( 3, 5, 7, 9 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a -= math::Vec4f( 1, 2, 3, 4 );
    ASSERT_EQUAL( a, math::Vec4f( 1, 1, 1, 1 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a *= math::Vec4f( 1, 2, 3, 4 );
    ASSERT_EQUAL( a, math::Vec4f( 2, 6, 12, 20 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a /= math::Vec4f( 1, 2, 3, 4 );
    ASSERT_EQUAL( a, math::Vec4f( 2, 1.5f, 4.f / 3.f, 5.f / 4.f ) );
  }


  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    ASSERT_EQUAL( a + 2, math::Vec4f( 4, 5, 6, 7 ) );
    ASSERT_EQUAL( a - 2, math::Vec4f( 0, 1, 2, 3 ) );
    ASSERT_EQUAL( a * 2, math::Vec4f( 4, 6, 8, 10 ) );
    ASSERT_EQUAL( a / 2, math::Vec4f( 1, 1.5f, 2, 2.5f ) );
  }

  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a += 2;
    ASSERT_EQUAL( a, math::Vec4f( 4, 5, 6, 7 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a -= 2;
    ASSERT_EQUAL( a, math::Vec4f( 0, 1, 2, 3 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a *= 2;
    ASSERT_EQUAL( a, math::Vec4f( 4, 6, 8, 10 ) );
  }
  {
    auto a = math::Vec4f( 2, 3, 4, 5 );
    a /= 2;
    ASSERT_EQUAL( a, math::Vec4f( 1, 1.5f, 2, 2.5f ) );
  }
}


TEST( math_Vec_dot_cross )
{
  ASSERT_EQUAL( math::Vec3f( 1, 3, -5 ).dot( math::Vec3f( 4, -2, -1 ) ), 3.f );
  ASSERT_EQUAL( math::Vec4f( 1, 3, -5, 3 ).dot( math::Vec4f( 4, -2, -1, 2 ) ), 9.f );

  ASSERT_EQUAL( math::Vec3f( 2, -3, 1 ).cross( math::Vec3f( 4, -1, 5 ) ),
                math::Vec3f( -14, -6, 10 ) );
}

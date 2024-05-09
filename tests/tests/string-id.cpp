#include "unit_test_framework/unit_test_framework.hpp"
#include "core/common.hpp"

namespace
{
  StringHash hash( const char* str )
  {
    return StringId( str ).getHash();
  }
} // namespace

TEST( string_id )
{
  ASSERT_EQUAL( hash( "" ), static_cast<StringHash>( 14695981039346656037u ) );
  ASSERT_EQUAL( hash( "hallo wrodl!" ), static_cast<StringHash>( 2866415834716882256u ) );
  ASSERT_EQUAL( hash( "mr1f_8_0_0_0_0x00071df0_f0" ), static_cast<StringHash>( 716540771061702267u ) );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f.map-i2" ), static_cast<StringHash>( 9581804885632933782u ) );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f-pp" ), static_cast<StringHash>( 5159170790092484585u ) );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2f-pp" ), static_cast<StringHash>( 11527482120388748660u ) );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2e-pp" ), static_cast<StringHash>( 2077102600007461255u ) );
}

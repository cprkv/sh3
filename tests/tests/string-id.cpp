#include "unit_test_framework/unit_test_framework.hpp"
#include "core/data/string-id.hpp"

namespace
{
  data::StringHash hash( const char* str )
  {
    return data::StringId( str ).getHash();
  }
} // namespace

TEST( string_id )
{
  ASSERT_EQUAL( hash( "" ), ( data::StringHash ) 14695981039346656037 );
  ASSERT_EQUAL( hash( "hallo wrodl!" ), ( data::StringHash ) 2866415834716882256 );
  ASSERT_EQUAL( hash( "mr1f_8_0_0_0_0x00071df0_f0" ), ( data::StringHash ) 716540771061702267 );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f.map-i2" ), ( data::StringHash ) 9581804885632933782 );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f-pp" ), ( data::StringHash ) 5159170790092484585 );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2f-pp" ), ( data::StringHash ) 11527482120388748660 );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2e-pp" ), ( data::StringHash ) 2077102600007461255 );
}

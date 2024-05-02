#include "unit_test_framework/unit_test_framework.hpp"
#include "core/data/string-id.hpp"

namespace
{
  core::data::StringHash hash( const char* str )
  {
    return core::data::StringId( str ).getHash();
  }
} // namespace

TEST( string_id )
{
  ASSERT_EQUAL( hash( "" ), ( core::data::StringHash ) 14695981039346656037u );
  ASSERT_EQUAL( hash( "hallo wrodl!" ), ( core::data::StringHash ) 2866415834716882256u );
  ASSERT_EQUAL( hash( "mr1f_8_0_0_0_0x00071df0_f0" ), ( core::data::StringHash ) 716540771061702267u );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f.map-i2" ), ( core::data::StringHash ) 9581804885632933782u );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr1f-pp" ), ( core::data::StringHash ) 5159170790092484585u );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2f-pp" ), ( core::data::StringHash ) 11527482120388748660u );
  ASSERT_EQUAL( hash( "X0/MR1F-MFA/mr2e-pp" ), ( core::data::StringHash ) 2077102600007461255u );
}

/**
 * @file ueb.cpp
 * Unit testing for the UEB rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/make_shared.hpp>

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

// ===================================================================
// NUMBERS
//
#define X(whole, decimal, result) \
  { \
    MDE_Number n(S(whole), S(decimal)); \
    REQUIRE(r.renderNumber(&n) == result); \
  }

TEST_CASE("render/ueb/numbers", "[render][ueb]") {
  UEBRenderer r;

  SECTION ("positive integers") { X("100", "", "#AJJ") }
  SECTION ("negative integers") { X("-100", "", "-#AJJ") }
  SECTION ("positive decmial + leading digits") { X("1", "25", "#A4BE") }
  SECTION ("negative decmial + leading digits") { X("-1", "25", "-#A4BE") }
  SECTION ("positive decmial w/o leading digits") { X("", "25", "#4BE") }
  SECTION ("negative decmial w/o leading digits") { X("-", "25", "-#4BE") }

  SECTION ("letter indicators for a-j when appearing after numbers") {
    for (char c = 'a'; c <= 'j'; c++) {
      MDEVector v;
      v.push_back (boost::make_shared<MDE_Number>(S("10"), S("")));
      v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
      
      std::string output = r.renderVector(v);
      REQUIRE (output == S("#AJ;" + CHARSTR(c)));
    }
  }

  SECTION ("no letter indicators for k-z when appearing after numbers") {
    for (char c = 'k'; c <= 'z'; c++) {
      MDEVector v;
      v.push_back (boost::make_shared<MDE_Number>(S("10"), S("")));
      v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
      
      std::string output = r.renderVector(v);
      REQUIRE (output == S("#AJ" + CHARSTR(c)));
    }
  }
}


#undef TEST

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

  // UEB Rulebook 6.1 ----------------------------------------------
  SECTION ("number mapping is correct") { X("0123456789", "", "#JABCDEFGHI") }

  // Examples: 100, -100, 1.25, -1.25, .25, -.25
  SECTION ("positive integers") { X("100", "", "#AJJ") }
  SECTION ("negative integers") { X("-100", "", "-#AJJ") }
  SECTION ("positive decmial + leading digits") { X("1", "25", "#A4BE") }
  SECTION ("negative decmial + leading digits") { X("-1", "25", "-#A4BE") }
  SECTION ("positive decmial w/o leading digits") { X("", "25", "#4BE") }
  SECTION ("negative decmial w/o leading digits") { X("-", "25", "-#4BE") }

  // UEB Rulebook 6.5 ----------------------------------------------
  // Scenario 1: Letter follows number
  //
  SECTION ("where a number ends in a digit") {
    MDEVector v;
    v.push_back (boost::make_shared<MDE_Number>(S("10"), S("")));

    // Example: 10a
    SECTION ("and a-j follow, expect letter indicator") {
      for (char c = 'a'; c <= 'j'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ;" + CHARSTR(c)));
	v.pop_back();
      }
    }

    // Example: 10k
    SECTION ("and k-z follow, no letter indicator required") {
      for (char c = 'k'; c <= 'z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ" + CHARSTR(c)));
	v.pop_back();
      }
    }

    // Example: 10C
    SECTION ("and A-Z (capitals) follow, no letter indicator required") {
      for (char c = 'A'; c <= 'Z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ," + CHARSTR(c)));
	v.pop_back();
      }
    }
  }
    
  // Scenario 2: Letter follows a period at the end of a number
  //
  SECTION ("where a number ends in a period") {
    MDEVector v;
    v.push_back (boost::make_shared<MDE_Number>(S("10"), S("")));
    v.push_back (boost::make_shared<MDE_MathBlock>(S(".")));

    // Example: 10.a
    SECTION ("and a-j follow, expect letter indicator") {
      for (char c = 'a'; c <= 'j'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ4;" + CHARSTR(c)));
	v.pop_back();
      }
    }
    
    // Example: 10.k
    SECTION ("and k-z follow, no letter indicator required") {
      for (char c = 'k'; c <= 'z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ4" + CHARSTR(c)));
	v.pop_back();
      }
    }

    // Example: 10.M
    SECTION ("and A-Z (capitals) follow, no letter indicator required") {
      for (char c = 'A'; c <= 'Z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ," + CHARSTR(c)));
	v.pop_back();
      }
    }
  }
}


#undef TEST

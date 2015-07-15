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
#define X(neg, whole, decimal, result)		\
  { \
    MDE_Number n(neg, S(whole), S(decimal));	\
    CHECK(r.renderNumber(&n) == result); \
  }

TEST_CASE("render/ueb/numbers", "[render][ueb]") {
  UEBRenderer r;

  // UEB Rulebook 6.1 ----------------------------------------------
  SECTION ("number mapping is correct") { X(false, "0123456789", "", "#JABCDEFGHI") }

  // Examples: 100, -100, 1.25, -1.25, .25, -.25
  SECTION ("positive integers") { X(false, "100", "", "#AJJ") }
  SECTION ("negative integers") { X(true, "100", "", "-#AJJ") }
  SECTION ("positive decmial + leading digits") { X(false, "1", "25", "#A4BE") }
  SECTION ("negative decmial + leading digits") { X(true, "1", "25", "-#A4BE") }
  SECTION ("positive decmial w/o leading digits") { X(false, "", "25", "#4BE") }
  SECTION ("negative decmial w/o leading digits") { X(true, "", "25", "-#4BE") }
}

TEST_CASE("render/ueb/numbers/divided", "[render][ueb]") {
  UEBRenderer r;

  // UEB Rulebook 6.2.1 (commas) ------------------------------------------
  // Example: 1,024
  SECTION ("thousands dividers (commas) are correct") { X(false, "1,024", "", "#A1JBD") }

  // UEB Rulebook 6.6.1 (numeric space)------------------------------------
  // Example: 1 024
  SECTION ("numeric spaces are correct, lhs") { X(false, "1 024", "", "#A\"JBD") }
  // Example: 3.141 592 653
  SECTION ("numeric spaces are correct, rhs") { X(false, "3", "141 592 653", "#C4ADA\"EIB\"FEC") }
}

TEST_CASE("render/ueb/numbers/followedByLetters", "[render][ueb]") {
  UEBRenderer r;

  // UEB Rulebook 6.5 ----------------------------------------------
  // Scenario 1: Letter follows number
  // 
  SECTION ("where a number ends in a digit") {
    MDEVector v;
    v.push_back (boost::make_shared<MDE_Number>(false, S("10"), S("")));

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
    v.push_back (boost::make_shared<MDE_Number>(false, S("10"), S("")));
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
	REQUIRE (r.renderVector(v) == S("#AJ4," + CHARSTR(c)));
	v.pop_back();
      }
    }
  }

  // Scenario 3: Letter follows a comma at the end of a number
  //
  SECTION ("where a number ends in a comma") {
    MDEVector v;
    v.push_back (boost::make_shared<MDE_Number>(false, S("10"), S("")));
    v.push_back (boost::make_shared<MDE_MathBlock>(S(",")));

    // Example: 10.a
    SECTION ("and a-j follow, expect letter indicator") {
      for (char c = 'a'; c <= 'j'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ1;" + CHARSTR(c)));
	v.pop_back();
      }
    }
    
    // Example: 10.k
    SECTION ("and k-z follow, no letter indicator required") {
      for (char c = 'k'; c <= 'z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ1" + CHARSTR(c)));
	v.pop_back();
      }
    }

    // Example: 10.M
    SECTION ("and A-Z (capitals) follow, no letter indicator required") {
      for (char c = 'A'; c <= 'Z'; c++) {
	v.push_back (boost::make_shared<MDE_MathBlock>(CHARSTR(c)));
	REQUIRE (r.renderVector(v) == S("#AJ1," + CHARSTR(c)));
	v.pop_back();
      }
    }
  }
}


#undef TEST

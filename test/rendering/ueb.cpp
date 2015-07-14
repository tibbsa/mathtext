/**
 * @file ueb.cpp
 * Unit testing for the UEB rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

/* ------------------ BASIC NUMBERS --------------------------------- */

TEST_CASE("render/ueb/numbers", "[render][ueb]") {
  UEBRenderer r;

  SECTION ("positive integers") {
    MDE_Number n(S("100"), S());
    REQUIRE(r.renderNumber(&n) == "#AJJ");
  }
  SECTION ("negative integers") {
    MDE_Number n(S("-100"), S());
    REQUIRE(r.renderNumber(&n) == "-#AJJ");
  }
  SECTION ("positive decimal number with leading digits") {
    MDE_Number n(S("1"), S("25"));
    REQUIRE(r.renderNumber(&n) == "#A4BE");
  }
  SECTION ("positive decimal number with no leading digits") {
    MDE_Number n(S(""), S("25"));
    REQUIRE(r.renderNumber(&n) == "#4BE");
  }
  SECTION ("negative decimal number with leading digits") {
    MDE_Number n(S("-1"), S("25"));
    REQUIRE(r.renderNumber(&n) == "-#A4BE");
  }
  SECTION ("negative decimal number with no leading digits") {
    MDE_Number n(S("-"), S("25"));
    REQUIRE(r.renderNumber(&n) == "-#4BE");
  }
}

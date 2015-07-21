/**
 * @file UEB_Fractions.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define MKNUM(pos, whole, dec) boost::make_shared<MDE_Number>(MDE_Number::pos, whole, dec)
#define MKTEXT(txt) boost::make_shared<MDE_MathBlock>(txt)
#define TF(num, den, brl) { MDEVector numerator, denominator; numerator.push_back(num); denominator.push_back(den); MDE_Fraction frac(numerator, denominator); CHECK(r.renderFraction(&frac) == brl); }

TEST_CASE("render/ueb/Fractions", "[render][UEB][Fractions]") {
  UEBRenderer r;

  // UEB Technical Guideline 6.1 --------------------------------------------
  // "Simple fractions" (those where both the numerator and denominator 
  // consist SOLELY of numbers) are rendered simple as [num]/[den].
  // 
  // We need to test a variety of number combinations to make sure that 
  // the fraction rendering code is detecting 'simple' fractions 
  // appropriately.  
  //
  // These examples are drawn from Technical Guideline 6.5
  SECTION ("Simple numeric fractions") {
    SECTION ("Simple numbers") {
      TF(MKNUM(POSITIVE, "3", ""), MKNUM(POSITIVE, "4", ""), "#C/D");
      TF(MKNUM(POSITIVE, "73", ""), MKNUM(POSITIVE, "4", ""), "#GC/D");
    }
    SECTION ("Numbers involving decimals") {
      TF(MKNUM(POSITIVE, "3", ""), MKNUM(POSITIVE, "4", "2"), "#C/D4B");
      TF(MKNUM(POSITIVE, "5", "3"), MKNUM(POSITIVE, "4200", ""), "#E4C/DBJJ");
    }
    SECTION ("Numbers involving numeric commas") {
      TF(MKNUM(POSITIVE, "5", "3"), MKNUM(POSITIVE, "4,200", ""), "#E4C/D1BJJ");
    }
    SECTION ("Numbers involving numeric spaces") {
      TF(MKNUM(POSITIVE, "5", "3"), MKNUM(POSITIVE, "4 200", ""), "#E4C/D\"BJJ");
    }
  }

  // UEB Technical Guideline 6.4 --------------------------------------------
  // Fractions where the numerator or denominator is not a pure number 
  // must be enclosed in fraction indicator symbols.
  //
  // As above, these examples are drawn from the examples in 6.5.
  //
  SECTION ("Fractions requiring general fraction indicators") {
    SECTION ("Fractions containing letters") {
      TF(MKTEXT("x"), MKTEXT("y"), "(x./y)");
      TF(MKTEXT("x"), MKNUM(POSITIVE, "4", ""), "(x./#D)");
      TF(MKNUM(POSITIVE, "3", ""), MKTEXT("b"), "(#C./b)");
    }

    // More complex examples, all of which should be treated as requiring 
    // general fraction indicators.

    // Example: @3^2~6#
    SECTION ("Complex example: Exponent in numerator term") {
      MDEVector numerator, denominator;
      MDEVector exponent_vector;

      exponent_vector.push_back (MKNUM(POSITIVE, "2", ""));
      numerator.push_back (MKNUM(POSITIVE, "3", ""));
      numerator.push_back (boost::make_shared<MDE_Exponent>(exponent_vector));
      denominator.push_back (MKNUM(POSITIVE, "6", ""));

      MDE_Fraction frac(numerator, denominator); 
      CHECK(r.renderFraction(&frac) == "(#C9#B./#F)");
    }
  }


}




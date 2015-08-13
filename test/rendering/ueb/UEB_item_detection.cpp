/**
 * @file item_detection.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

// Modifiers, exponents, and subscripts can appear in braille between
// grouping indicators or without them, depending on whether the argument
// or expression involved is an "item" or not.
//
// UEB Technical Guidelines ss. 7.1, 12.1
//
// In general, an item is: a number; a fraction; a radical; a narrow; a shape;
//  anything in paranthesees, square brackets, or braces; anything in
//  braille grouping indicators.

// For testing purposes, we use an exponent as the exmaple

// Technical Guidelines 7.1, item #1 (numbers)
TEST_CASE("render/ueb/item_detection/numbers", "[render][UEB]") {
  UEBRenderer r;

  SECTION("items that are plain numbers") {
    // Example: 2 (as in x^2)
    SECTION("with a single digit") {
      MDEVector v;
      v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));
      CHECK(r.isBrailleItem(v) == true);
    }

    // Example: 24 (as in x^24)
    SECTION("with multiple digits") {
      MDEVector v;
      v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "24", ""));
      CHECK(r.isBrailleItem(v) == true);
    }

    // Example: 3.14 (as in x^3.14)
    SECTION ("with decimals") {
      MDEVector v;
      v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "3", "14"));
      CHECK(r.isBrailleItem(v) == true);
    }

    // Example: 2 048 (as in x^2 048)
    // Example: 3.14 (as in x^3.14)
    SECTION ("with numeric spaces") {
      MDEVector v;
      v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2 048", ""));
      CHECK(r.isBrailleItem(v) == true);
    }

    // Example: 1,024 (as in x^1,024)
    SECTION ("with numeric commas") {
      MDEVector v;
      v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "1,048", ""));
      CHECK(r.isBrailleItem(v) == true);
    }
  }
}


// Technical Guidelines 7.1, item #1 (simple fractions), #2 (general fractions)
TEST_CASE("render/ueb/item_detection/fractions", "[render][UEB]") {
  UEBRenderer r;

  // Example: @1~2#
  SECTION("simple numeric fractions") {
    MDEVector v;
    MDEVector fracNum, fracDen;
    fracNum.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "1", ""));
    fracDen.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));
    v.push_back (boost::make_shared<MDE_Fraction>(fracNum, fracDen));

    CHECK(r.isBrailleItem(v) == true);
  }

  // Example: @x~y#
  SECTION("general fractions") {
    MDEVector v;
    MDEVector fracNum, fracDen;
    fracNum.push_back (boost::make_shared<MDE_MathBlock>("x"));
    fracDen.push_back (boost::make_shared<MDE_MathBlock>("y"));
    v.push_back (boost::make_shared<MDE_Fraction>(fracNum, fracDen));

    CHECK(r.isBrailleItem(v) == true);
  }
}

// Technical Guidelines 7.1, item #3 (radical expression)
TEST_CASE("render/ueb/item_detection/roots", "[render][UEB]") {
  UEBRenderer r;

  // Example: _/4
  MDEVector rootArgument, rootIndex;
  rootArgument.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

  MDEVector v;
  v.push_back(boost::make_shared<MDE_Root>(rootIndex, rootArgument));
  CHECK(r.isBrailleItem(v) == true);
}

// Technical guidelines 7.1, item #8 ('the next individual symbol')

//## TODO: greek letters
//## TODO: operators
//## TODO: other symbols?

//
// Items below here should NOT be treated as items
//
TEST_CASE("render/ueb/item_detection/non-items", "[render][UEB]") {
  UEBRenderer r;

  SECTION ("items containing multiple elements") {
    // Example: 2x (any item containing more than one element)
    MDEVector v;
    v.push_back(boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));
    v.push_back(boost::make_shared<MDE_MathBlock>("x"));
    CHECK(r.isBrailleItem(v) == false);
  }

  SECTION ("negative numbers") {
    // Example: negative numbers
    // -1
    MDEVector v;
    v.push_back(boost::make_shared<MDE_Number>(MDE_Number::NEGATIVE, "1", ""));
    CHECK(r.isBrailleItem(v) == false);
  }
}

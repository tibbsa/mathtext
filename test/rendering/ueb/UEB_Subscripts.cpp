/**
 * @file UEB_Subscripts.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

// Subscripts can appear with or without grouping indicators.  Grouping
// indicators are not required for braille items that are considered
// "items".  We're not interested here in testing whether item detection
// works properly for all test cases -- only that when items appear,
// grouping indicators are ignored.  Item detection tests are separate, under
// 'render/ueb/item_detection'.  Here, we use a single number (2) to serve
// as a simple "item", and a negative number (-2) to serve as an item
// requiring grouping indicators (since the minus sign can itself be an
// item).


TEST_CASE("render/ueb/Subscripts", "[render][UEB][Subscripts]") {
  UEBRenderer r;

  // Example: x_2;y (no grouping required, no letter indicator after exponent)
  // Example: x_2;b (no grouping required, but letter indicator needed)
  SECTION ("Subscripts not requiring grouping indicators") {
    MDEVector subscript_argument;
    subscript_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));

    MDEVector expression;
    expression.push_back (boost::make_shared<MDE_MathBlock>("x"));
    expression.push_back (boost::make_shared<MDE_Subscript>(subscript_argument));

    SECTION ("and followed by a letter not requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("y"));

      CHECK (r.renderVector (expression) == "x5#By");
    }

    SECTION ("and followed by a letter potentially requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("b"));

      CHECK (r.renderVector (expression) == "x5#B;b");
    }
  }


  // Example: x_(-1)y (grouping required, no letter indicator after exponent)
  // Example: x_(-1)b (grouping required, no letter indicator after exponent)
  SECTION ("Subscripts requiring grouping indicators") {
    MDEVector subscript_argument;
    subscript_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::NEGATIVE, "1", ""));

    MDEVector expression;
    expression.push_back (boost::make_shared<MDE_MathBlock>("x"));
    expression.push_back (boost::make_shared<MDE_Subscript>(subscript_argument));

    SECTION ("and followed by a letter not requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("y"));

      CHECK (r.renderVector (expression) == "x5<-#A>y");
    }

    SECTION ("and followed by a letter potentially requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("b"));

      CHECK (r.renderVector (expression) == "x5<-#A>b");
    }
  }
}




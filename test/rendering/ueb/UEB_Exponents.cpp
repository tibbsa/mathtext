/**
 * @file UEB_Exponents.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

// Exponents can appear with or without grouping indicators.  Grouping 
// indicators are not required for braille items that are considered 
// "items".  We're not interested here in testing whether item detection 
// works properly for all test cases -- only that when items appear, 
// grouping indicators are ignored.  Item detection tests are separate, under
// 'render/ueb/item_detection'.  Here, we use a single number (2) to serve
// as a simple "item", and a negative number (-2) to serve as an item 
// requiring grouping indicators (since the minus sign can itself be an 
// item).


TEST_CASE("render/ueb/Exponents", "[render][UEB][Exponents]") {
  UEBRenderer r;

  // Example: x^2;y (no grouping required, no letter indicator after exponent)
  // Example: x^2;b (no grouping required, but letter indicator needed)
  SECTION ("Exponents not requiring grouping indicators") {
    MDEVector exponent_argument;
    exponent_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));
    
    MDEVector expression;
    expression.push_back (boost::make_shared<MDE_MathBlock>("x"));
    expression.push_back (boost::make_shared<MDE_Exponent>(exponent_argument));
   
    SECTION ("and followed by a letter not requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("y"));

      CHECK (r.renderVector (expression) == "x9#By");
    }

    SECTION ("and followed by a letter potentially requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("b"));

      CHECK (r.renderVector (expression) == "x9#B;b");
    }
  }


  // Example: x^(-1)y (grouping required, no letter indicator after exponent)
  // Example: x^(-1)b (grouping required, no letter indicator after exponent)
  SECTION ("Exponents requiring grouping indicators") {
    MDEVector exponent_argument;
    exponent_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::NEGATIVE, "1", ""));

    MDEVector expression;
    expression.push_back (boost::make_shared<MDE_MathBlock>("x"));
    expression.push_back (boost::make_shared<MDE_Exponent>(exponent_argument));

    SECTION ("and followed by a letter not requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("y"));

      CHECK (r.renderVector (expression) == "x9<-#A>y");
    }

    SECTION ("and followed by a letter potentially requiring a letter indicator") {
      expression.push_back (boost::make_shared<MDE_MathBlock>("b"));

      CHECK (r.renderVector (expression) == "x9<-#A>b");
    }
  }
}




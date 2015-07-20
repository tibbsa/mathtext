/**
 * @file UEB_Roots.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

TEST_CASE("render/ueb/Roots", "[render][UEB][Roots]") {
  UEBRenderer r;

  // UEB Technical Guideline 8.1 --------------------------------------------
  // The simplest square root case: _/4 
  // (but any argument, no matter how complex, will be the same)
  SECTION ("Standard square roots") {
    MDEVector root_index;
    MDEVector root_argument;
    root_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

    const MDE_Root root (root_index, root_argument);
    CHECK (r.renderRoot (&root) == "%#D+");
  }

  // UEB Technical Guideline 8.2 ---------------------------------------------
  // Roots with 'indexes' are presented by putting an exponent-like expression
  // at the start of the root argument.  For simple 'items' (see the 
  // item_detection tests for more information), these do not need grouping 
  // indicators.

  SECTION ("Root with a simple numeric index") {
    MDEVector root_index;
    MDEVector root_argument;
    root_index.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "3", ""));
    root_argument.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "27", ""));

    const MDE_Root root (root_index, root_argument);
    CHECK (r.renderRoot (&root) == "%9#C#BG+");
  }

  // More complex root indexes need grouping indicators.  See the example
  // in the technical guidelines for: _/[mn](xy) (the mn-th root of xy)
  SECTION ("Root with a complex index") {
    MDEVector root_index;
    MDEVector root_argument;
    root_index.push_back (boost::make_shared<MDE_MathBlock>("mn"));
    root_argument.push_back (boost::make_shared<MDE_MathBlock>("xy"));

    const MDE_Root root (root_index, root_argument);
    CHECK (r.renderRoot (&root) == "%9<mn>xy+");
  }
}




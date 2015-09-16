/**
 * @file Comparators.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define RENDERCHECK(comp,brl) { MDE_Comparator c(comp); CHECK(r.renderComparator(&c) == brl); }

TEST_CASE("render/ueb/Comparators", "[render][UEB][Comparator]") {
  UEBRenderer r;

  // UEB Rulebook 3.17, Technical Guidelines 3 ----------------------------
  RENDERCHECK(MDE_Comparator::LESS_THAN, " @< ");
  RENDERCHECK(MDE_Comparator::GREATER_THAN, " @> ");
  RENDERCHECK(MDE_Comparator::EQUALS, " \"7 ");
  RENDERCHECK(MDE_Comparator::APPROX_EQUALS, " _9 ");
  RENDERCHECK(MDE_Comparator::NOT_EQUALS, " \"7@: ");
  RENDERCHECK(MDE_Comparator::GREATER_THAN_EQUALS, " _@> ");
  RENDERCHECK(MDE_Comparator::LESS_THAN_EQUALS, " _@< ");
}




/**
 * @file ItemNumbers.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define RENDERCHECK(str,brl) { MDE_ItemNumber t(str); CHECK(r.renderItemNumber(&t) == brl); }

TEST_CASE("render/ueb/ItemNumber", "[render][UEB][ItemNumber]") {
  UEBRenderer r;

  // UEB Rulebook 6.1 (number signs).
  RENDERCHECK("1.", "#a4 ");
  RENDERCHECK("1)", "#a\"> ");
  RENDERCHECK("(1)", "\"<#a\"> ");

  // UEB Rulebook 2.6.2/2.6.3 (re letter indicators), 7 (paranthesees)
  RENDERCHECK("a.", "a4 ");
  RENDERCHECK("c)", ";c\"> ");
  RENDERCHECK("(b)", "\"<;b\"> ");
}


/**
 * @file UEB_Symbols.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/make_shared.hpp>

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define R(sym,brl) { MDE_Symbol s(sym); CHECK(r.renderSymbol(&s) == brl); }

TEST_CASE("render/ueb/Symbols", "[render][UEB][Symbol]") {
  UEBRenderer r;

  // UEB Rulebook 7, Technical Guidelines 5---------------------------------
  R(MDE_Symbol::COMMA, "1 ");
  R(MDE_Symbol::FACTORIAL, "6");
  R(MDE_Symbol::LEFT_BRACE, "_<");
  R(MDE_Symbol::LEFT_BRACKET, ".<");
  R(MDE_Symbol::LEFT_PAREN, "\"<"); 
  R(MDE_Symbol::RIGHT_BRACE, "_>");
  R(MDE_Symbol::RIGHT_BRACKET, ".>");
  R(MDE_Symbol::RIGHT_PAREN, "\">");
  
  // UEB Rulebook 3.21, Technical Guidelines 2.10 ---------------------------
  R(MDE_Symbol::PERCENT, ".0");

  // Technical Gudelines 11 -------------------------------------------------
  R(MDE_Symbol::THEREFORE, ",*");

}




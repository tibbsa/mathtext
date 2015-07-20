/**
 * @file Operators.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define RENDERCHECK(oper,brl) { MDE_Operator op(oper); CHECK(r.renderOperator(&op) == brl); }

TEST_CASE("render/ueb/Operators/unspaced", "[render][UEB][Operator]") {
  UEBRenderer r;

  // Disable 'extra operator spacing' 
  MDE_Command setupCmd("ExtraOperatorSpacing", "false");
  r.renderCommand(&setupCmd);
  
  // UEB Rulebook 11.2 ---------------------------------------------
  RENDERCHECK(MDE_Operator::ADDITION, "\"6");
  RENDERCHECK(MDE_Operator::SUBTRACTION, "\"-");
  RENDERCHECK(MDE_Operator::MULTIPLICATION, "\"8");
  RENDERCHECK(MDE_Operator::DIVISION, "\"/");
}

TEST_CASE("render/ueb/Operators/spaced", "[render][UEB][Operator]") {
  UEBRenderer r;

  // Enable 'extra operator spacing' 
  MDE_Command setupCmd("ExtraOperatorSpacing", "true");
  r.renderCommand(&setupCmd);

  // UEB Rulebook 11.2.2 -------------------------------------------
  RENDERCHECK(MDE_Operator::ADDITION, " \"6 ");
  RENDERCHECK(MDE_Operator::SUBTRACTION, " \"- ");
  RENDERCHECK(MDE_Operator::MULTIPLICATION, " \"8 ");
  RENDERCHECK(MDE_Operator::DIVISION, " \"/ ");
}



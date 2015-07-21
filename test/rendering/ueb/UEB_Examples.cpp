/**
 * @file UEB_Examples.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"
#include "UEBRenderer.h"

//
// These are example expressions that are tested from source to rendering
// completely.  Many of the examples are derived from the UEB manual or
// technical guildelines.
//
void executeTest (const std::string &input, const std::string &output)
{
  CAPTURE(input);
  CAPTURE(output);

  MathSourceFile src;
  REQUIRE_NOTHROW(src.loadFromBuffer (input));

  MathDocument document;
  MathInterpreter interpreter(src, document);
  REQUIRE_NOTHROW(interpreter.interpret());
  REQUIRE(interpreter.haveMessages() == false);

  UEBRenderer r;
  std::string generatedOutput;
  REQUIRE_NOTHROW(generatedOutput = r.renderDocument(document));
  CHECK(generatedOutput == (output + "\n"));
}

TEST_CASE("render/ueb/examples/supersubscripts", "[render][UEB][examples]") {
  SECTION("UEB Technical Guidelines, Section 7.3 (Algebra)") {
    executeTest("x^2", "x9#B");
    executeTest("x^2;y", "x9#By");
    executeTest("x^(2y)", "x9<#By>");
    executeTest("x^y;+1", "x9y\"6#A");
    executeTest("x^(y+1)", "x9<y\"6#A>");
    executeTest("x^(y+1)+3", "x9<y\"6#A>\"6#C");
    executeTest("x^@2~3#", "x9#B/C");
    executeTest("@x^2~3#", "(x9#B./#C)");
    executeTest("x^(@1~2#y)", "x9<#A/By>");
    executeTest("x^@1~2#y", "x9#A/By");
    executeTest("x^@a~b#y=x", "x9(a./b)y \"7 x");
    executeTest("&If $@x^2+2x~1+x^2# = 1\n" 
                "x^2 + 2x = 1 + x^2\n"
                "x = @1~2#", 
		   ",if (x9#B\"6#Bx./#A\"6x9#B) \"7 #A\n"
                   "x9#B\"6#Bx \"7 #A\"6x9#B\n"
		   "x \"7 #A/B");
  }
  SECTION("UEB Technical Guidelines, Section 7.4 (Multiple levels)") {
    executeTest("e^(x^2)", "e9<x9#B>");
    // NOT IMPLEMENTED; executeTest("e^(x^2)", "e9\"<x9#B\">");
    executeTest("P_(x_i)", ",P5<x5i>");
  }
  SECTION("UEB Technical Guidelines, Section 7.5 (Negative superscripts)") {
    executeTest("0.0045 = 4.5*10^-3", "#J4JJDE \"7 #D4E\"8#AJ9<\"-#C>");
    executeTest("v = 60ms^-1", "v \"7 #FJms9<\"-#A>");
    executeTest("a^(-2b)", "a9<\"-#B;b>");
  }

  SECTION("contrived examples") {
    executeTest("x^2 + 2x^@1~2# = z_2", "x9#B\"6#Bx9#A/B \"7 z5#B");
  }
}


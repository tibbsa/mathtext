/**
 * @file UEB_examples.cpp
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
// Many of these examples, while focused on a particular area (e.g. numbers),
// will also test other aspects of the translation engine, because they
// include significant other symbols, or they include text. This is why these
// examples are separate and apart from the more targetted indiivdual tests
// for each element type.
//
namespace {
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
}

// =========================================================================
TEST_CASE("render/ueb/examples/numbers", "[render][UEB][examples][Numbers]") {
  SECTION("UEB Technical Guidelines, Section 2.1 (Whole numbers)") {
    executeTest("456", "#DEF");
    executeTest("3,000", "#C1JJJ");
    executeTest("5 000 000", "#E\"JJJ\"JJJ");
  }

  SECTION("UEB Technical Guidelines, Section 2.2 (Decimals)") {
    executeTest("8.93", "#H4IC");
    executeTest("0.7", "#J4G");
    executeTest(".7", "#4G");
    executeTest(".8 &is a decimal fraction.", "#4H is a decimal fraction4");
  }

}


//
// =========================================================================
// The UEB guidelines give tons of examples for exponents but very few for
// subscripts. Since the code is not entirely shared, we duplicate the
// exponent examples for subscripts notwithstanding that you wouldn't, in
// typical real-world scenarios, see these in subscript form.
TEST_CASE("render/ueb/examples/superscripts", "[render][UEB][examples][Exponents]") {
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
    // An expression in paranthesees, square brackets, or braces can be taken
    // as an item, but for that to work in this context, parsing of
    // bracketed groups (i.e. in (x-1)^2, (x-1) would have to be an element
    // on its own rather than [lparen symbol][x][-][1][rparen symbol]
    executeTest("P^(x^i)", ",P9<x9i>");
  }
  SECTION("UEB Technical Guidelines, Section 7.5 (Negative superscripts)") {
    executeTest("0.0045 = 4.5*10^-3", "#J4JJDE \"7 #D4E\"8#AJ9<\"-#C>");
    executeTest("v = 60ms^-1", "v \"7 #FJms9<\"-#A>");
    executeTest("a^(-2b)", "a9<\"-#B;b>");
  }
}

TEST_CASE("render/ueb/examples/subscripts", "[render][UEB][examples][Subscripts]") {
  SECTION("UEB Technical Guidelines, Section 7.3 (Algebra)") {
    executeTest("x_2", "x5#B");
    executeTest("x_2;y", "x5#By");
    executeTest("x_(2y)", "x5<#By>");
    executeTest("x_y;+1", "x5y\"6#A");
    executeTest("x_(y+1)", "x5<y\"6#A>");
    executeTest("x_(y+1)+3", "x5<y\"6#A>\"6#C");
    executeTest("x_@2~3#", "x5#B/C");
    executeTest("@x_2~3#", "(x5#B./#C)");
    executeTest("x_(@1~2#y)", "x5<#A/By>");
    executeTest("x_@1~2#y", "x5#A/By");
    executeTest("x_@a~b#y=x", "x5(a./b)y \"7 x");
    executeTest("&If $@x_2+2x~1+x_2# = 1\n"
                "x_2 + 2x = 1 + x_2\n"
                "x = @1~2#",
		   ",if (x5#B\"6#Bx./#A\"6x5#B) \"7 #A\n"
                   "x5#B\"6#Bx \"7 #A\"6x5#B\n"
		   "x \"7 #A/B");
  }
  SECTION("UEB Technical Guidelines, Section 7.4 (Multiple levels)") {
    executeTest("e_(x_2)", "e5<x5#B>");
    // NOT IMPLEMENTED; executeTest("e_((x_2))", "e9\"<x9#B\">");
    // An expression in paranthesees, square brackets, or braces can be taken
    // as an item, but for that to work in this context, parsing of
    // bracketed groups (i.e. in (x-1)_2, (x-1) would have to be an element
    // on its own rather than [lparen symbol][x][-][1][rparen symbol]
     executeTest("P_(x_i)", ",P5<x5i>");
  }
  SECTION("UEB Technical Guidelines, Section 7.5 (Negative subscripts)") {
    executeTest("0.0045 = 4.5*10_-3", "#J4JJDE \"7 #D4E\"8#AJ5<\"-#C>");
    executeTest("v = 60ms_-1", "v \"7 #FJms5<\"-#A>");
    executeTest("a_(-2b)", "a5<\"-#B;b>");
  }
}

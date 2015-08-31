/**
 * @file Int_Exponents.cpp
 *
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"

// =========================================================================
TEST_CASE("interpret/Exponents", "[interpret][Exponents]") {
  SECTION ("terminating on explicit terminator, space, comma, end of sequence") {
    checkInterpretation("x^10", "<M>x</M><EXP><#>10</#></EXP>");
    checkInterpretation("(x^2, y^2) = (a^2,b^2)", "<GROUP:(><M>x</M><EXP><#>2</#></EXP><COMMA><M> y</M><EXP><#>2</#></EXP></GROUP:)><equals><GROUP:(><M>a</M><EXP><#>2</#></EXP><COMMA><M>b</M><EXP><#>2</#></EXP></GROUP:)>");
    checkInterpretation("x^2 + a^2;b^2", "<M>x</M><EXP><#>2</#></EXP><plus><M>a</M><EXP><#>2</#></EXP><M>b</M><EXP><#>2</#></EXP>");
  }
  SECTION ("terminating on a sign of operation") {
    checkInterpretation("x^2+y", "<M>x</M><EXP><#>2</#></EXP><plus><M>y</M>");
    checkInterpretation("x^2-y", "<M>x</M><EXP><#>2</#></EXP><minus><M>y</M>");
    checkInterpretation("x^2*y", "<M>x</M><EXP><#>2</#></EXP><mult><M>y</M>");
    checkInterpretation("x^2 / y", "<M>x</M><EXP><#>2</#></EXP><div><M>y</M>");
  }
  SECTION ("terminating on a sign of comparison") {
    checkInterpretation("x^2<y", "<M>x</M><EXP><#>2</#></EXP><is less than><M>y</M>");
    checkInterpretation("x^2>y", "<M>x</M><EXP><#>2</#></EXP><is greater than><M>y</M>");
    checkInterpretation("x^2<=y", "<M>x</M><EXP><#>2</#></EXP><is less than or equal to><M>y</M>");
    checkInterpretation("x^2>=y", "<M>x</M><EXP><#>2</#></EXP><is greater than or equal to><M>y</M>");
    checkInterpretation("x^2!=y", "<M>x</M><EXP><#>2</#></EXP><is not equal to><M>y</M>");
    checkInterpretation("x^2~=y", "<M>x</M><EXP><#>2</#></EXP><approximately equals><M>y</M>");
  }
  SECTION("terminating on begin of new group") {
    checkInterpretation ("x^2(2x + 3y)", "<M>x</M><EXP><#>2</#></EXP><GROUP:(><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:)>");
    checkInterpretation ("x^2[2x + 3y]", "<M>x</M><EXP><#>2</#></EXP><GROUP:[><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:]>");
    checkInterpretation ("x^2{2x + 3y}", "<M>x</M><EXP><#>2</#></EXP><GROUP:{><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:}>");
  }
  SECTION("terminating on end of enclosing group") {
    checkInterpretation ("(x^2)", "<GROUP:(><M>x</M><EXP><#>2</#></EXP></GROUP:)>");
    checkInterpretation ("[x^2]", "<GROUP:[><M>x</M><EXP><#>2</#></EXP></GROUP:]>");
    checkInterpretation ("{x^2}", "<GROUP:{><M>x</M><EXP><#>2</#></EXP></GROUP:}>");
  }
  SECTION("terminating on start of fraction") {
    checkInterpretation ("x^4@1~2#", "<M>x</M><EXP><#>4</#></EXP><FRAC><#>1</#><OVER><#>2</#></FRAC>");
  }
  SECTION("terminating at end of numerator in fraction") {
    checkInterpretation ("@x^2~4#", "<FRAC><M>x</M><EXP><#>2</#></EXP><OVER><#>4</#></FRAC>");
    checkInterpretation ("@x^@1~2#~4#", "<FRAC><M>x</M><EXP><FRAC><#>1</#><OVER><#>2</#></FRAC></EXP><OVER><#>4</#></FRAC>");
  }
  SECTION("terminating at end of denominator in fraction") {
    checkInterpretation ("@4~x^2#+1", "<FRAC><#>4</#><OVER><M>x</M><EXP><#>2</#></EXP></FRAC><plus><#>1</#>");
    checkInterpretation ("@4~x^@1~2##+1", "<FRAC><#>4</#><OVER><M>x</M><EXP><FRAC><#>1</#><OVER><#>2</#></FRAC></EXP></FRAC><plus><#>1</#>");
  }
}

TEST_CASE("interpret/Exponents/grouped", "[interpret][Exponents]") {
  checkInterpretation("x^(a+b*(c + d))", "<M>x</M><EXP><M>a</M><plus><M>b</M><mult><GROUP:(><M>c</M><plus><M>d</M></GROUP:)></EXP>");
}

TEST_CASE("interpret/Exponents/fractional", "[interpret][Exponents]") {
  checkInterpretation("x^@1~2#y", "<M>x</M><EXP><FRAC><#>1</#><OVER><#>2</#></FRAC></EXP><M>y</M>");
  checkInterpretation("x^@1~@1~z##", "<M>x</M><EXP><FRAC><#>1</#><OVER><FRAC><#>1</#><OVER><M>z</M></FRAC></FRAC></EXP>");
}

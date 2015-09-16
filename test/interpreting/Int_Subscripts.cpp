/**
 * @file Int_Subscripts.cpp
 *
 * This is a direct copy of Int_Exponents.cpp but with replacements to
 * replicate the same tests for subscripts.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "mttest.h"

// =========================================================================
TEST_CASE("interpret/Subscripts", "[interpret][Subscripts]") {
  SECTION ("terminating on explicit terminator, space, comma, end of sequence") {
    checkInterpretation("x^10", "<M>x</M><EXP><#>10</#></EXP>");
    checkInterpretation("(x_2, y_2) = (a_2,b_2)", "<GROUP:(><M>x</M><SUB><#>2</#></SUB><COMMA><M> y</M><SUB><#>2</#></SUB></GROUP:)><equals><GROUP:(><M>a</M><SUB><#>2</#></SUB><COMMA><M>b</M><SUB><#>2</#></SUB></GROUP:)>");
    checkInterpretation("x_2 + a_2;b_2", "<M>x</M><SUB><#>2</#></SUB><plus><M>a</M><SUB><#>2</#></SUB><M>b</M><SUB><#>2</#></SUB>");
  }
  SECTION ("terminating on a sign of operation") {
    checkInterpretation("x_2+y", "<M>x</M><SUB><#>2</#></SUB><plus><M>y</M>");
    checkInterpretation("x_2-y", "<M>x</M><SUB><#>2</#></SUB><minus><M>y</M>");
    checkInterpretation("x_2*y", "<M>x</M><SUB><#>2</#></SUB><mult><M>y</M>");
    checkInterpretation("x_2 / y", "<M>x</M><SUB><#>2</#></SUB><div><M>y</M>");
  }
  SECTION ("terminating on a sign of comparison") {
    checkInterpretation("x_2<y", "<M>x</M><SUB><#>2</#></SUB><is less than><M>y</M>");
    checkInterpretation("x_2>y", "<M>x</M><SUB><#>2</#></SUB><is greater than><M>y</M>");
    checkInterpretation("x_2<=y", "<M>x</M><SUB><#>2</#></SUB><is less than or equal to><M>y</M>");
    checkInterpretation("x_2>=y", "<M>x</M><SUB><#>2</#></SUB><is greater than or equal to><M>y</M>");
    checkInterpretation("x_2!=y", "<M>x</M><SUB><#>2</#></SUB><is not equal to><M>y</M>");
    checkInterpretation("x_2~=y", "<M>x</M><SUB><#>2</#></SUB><approximately equals><M>y</M>");
  }
  SECTION("terminating on begin of new group") {
    checkInterpretation ("x_2(2x + 3y)", "<M>x</M><SUB><#>2</#></SUB><GROUP:(><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:)>");
    checkInterpretation ("x_2[2x + 3y]", "<M>x</M><SUB><#>2</#></SUB><GROUP:[><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:]>");
    checkInterpretation ("x_2{2x + 3y}", "<M>x</M><SUB><#>2</#></SUB><GROUP:{><#>2</#><M>x</M><plus><#>3</#><M>y</M></GROUP:}>");
  }
  SECTION("terminating on end of enclosing group") {
    checkInterpretation ("(x_2)", "<GROUP:(><M>x</M><SUB><#>2</#></SUB></GROUP:)>");
    checkInterpretation ("[x_2]", "<GROUP:[><M>x</M><SUB><#>2</#></SUB></GROUP:]>");
    checkInterpretation ("{x_2}", "<GROUP:{><M>x</M><SUB><#>2</#></SUB></GROUP:}>");
  }
  SECTION("terminating on start of fraction") {
    checkInterpretation ("x_4@1~2#", "<M>x</M><SUB><#>4</#></SUB><FRAC><#>1</#><OVER><#>2</#></FRAC>");
  }
  SECTION("terminating at end of numerator in fraction") {
    checkInterpretation ("@x_2~4#", "<FRAC><M>x</M><SUB><#>2</#></SUB><OVER><#>4</#></FRAC>");
    checkInterpretation ("@x_@1~2#~4#", "<FRAC><M>x</M><SUB><FRAC><#>1</#><OVER><#>2</#></FRAC></SUB><OVER><#>4</#></FRAC>");
  }
  SECTION("terminating at end of denominator in fraction") {
    checkInterpretation ("@4~x_2#+1", "<FRAC><#>4</#><OVER><M>x</M><SUB><#>2</#></SUB></FRAC><plus><#>1</#>");
    checkInterpretation ("@4~x_@1~2##+1", "<FRAC><#>4</#><OVER><M>x</M><SUB><FRAC><#>1</#><OVER><#>2</#></FRAC></SUB></FRAC><plus><#>1</#>");
  }
}

TEST_CASE("interpret/Subscripts/grouped", "[interpret][Subscripts]") {
  checkInterpretation("x_(a+b*(c + d))", "<M>x</M><SUB><M>a</M><plus><M>b</M><mult><GROUP:(><M>c</M><plus><M>d</M></GROUP:)></SUB>");
}

TEST_CASE("interpret/Subscripts/fractional", "[interpret][Subscripts]") {
  checkInterpretation("x_@1~2#y", "<M>x</M><SUB><FRAC><#>1</#><OVER><#>2</#></FRAC></SUB><M>y</M>");
  checkInterpretation("x_@1~@1~z##", "<M>x</M><SUB><FRAC><#>1</#><OVER><FRAC><#>1</#><OVER><M>z</M></FRAC></FRAC></SUB>");
}

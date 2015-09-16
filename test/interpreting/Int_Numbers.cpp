/**
 * @file Int_Numbers.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "mttest.h"

// =========================================================================
TEST_CASE("interpret/Numbers/whole", "[interpret][Numbers]") {
  SECTION("simple") {
    checkInterpretation ("1", "<#>1</#>");
    checkInterpretation ("10", "<#>10</#>");
    checkInterpretation ("1048576", "<#>1048576</#>");
  }
  SECTION("with commas") {
    checkInterpretation ("1,024", "<#>1,024</#>");
    checkInterpretation ("1,048,576", "<#>1,048,576</#>");
    checkInterpretation ("10,048,576", "<#>10,048,576</#>");
    checkInterpretation ("100,048,576", "<#>100,048,576</#>");
  }
  SECTION("with spaces") {
    checkInterpretation ("1 024", "<#>1 024</#>");
    checkInterpretation ("1 048 576", "<#>1 048 576</#>");
    checkInterpretation ("10 048 576", "<#>10 048 576</#>");
    checkInterpretation ("100 048 576", "<#>100 048 576</#>");
  }
}

// =========================================================================
TEST_CASE("interpret/Numbers/decimal", "[interpret][Numbers]") {
  SECTION("simple") {
    checkInterpretation ("1.1", "<#>1.1</#>");
    checkInterpretation ("10.25", "<#>10.25</#>");
    checkInterpretation ("1048576.171274174", "<#>1048576.171274174</#>");
  }
  SECTION("with commas") {
    checkInterpretation ("1,024.25", "<#>1,024.25</#>");
    checkInterpretation ("1,048,576.3", "<#>1,048,576.3</#>");
    checkInterpretation ("10,048,576.224 116", "<#>10,048,576.224 116</#>");
    checkInterpretation ("100,048,576.72", "<#>100,048,576.72</#>");
    checkInterpretation ("12,248,57", "<#>12,248</#><COMMA><#>57</#>");
  }
  SECTION("with spaces") {
    checkInterpretation ("1 024.25", "<#>1 024.25</#>");
    checkInterpretation ("1 048 576.3", "<#>1 048 576.3</#>");
    checkInterpretation ("10 048 576.224 116", "<#>10 048 576.224 116</#>");
    checkInterpretation ("100 048 576.72", "<#>100 048 576.72</#>");
  }
}


// =========================================================================
TEST_CASE("interpret/Numbers/terminations", "[interpret][Numbers]") {
  SECTION("forced terminations (lhs)") {
    checkInterpretation ("101 102 103", "<#>101 102 103</#>");
    checkInterpretation ("101\\ 102 103", "<#>101</#><#>102 103</#>");
    checkInterpretation ("101 102\\ 103", "<#>101 102</#><#>103</#>");
  }
  SECTION("forced terminations (rhs)") {
    checkInterpretation ("0.101 102 103", "<#>0.101 102 103</#>");
    checkInterpretation ("0.101\\ 102 103", "<#>0.101</#><#>102 103</#>");
    checkInterpretation ("0.101 102\\ 103", "<#>0.101 102</#><#>103</#>");
  }
  SECTION("terminating on periods") {
    checkInterpretation ("1024.", "<#>1024</#><PERIOD>");
    checkInterpretation ("1024.x", "<#>1024</#><PERIOD><M>x</M>");
  }
  SECTION("terminating on spaces, letters, etc.") {
    checkInterpretation ("1024 = x", "<#>1024</#><equals><M>x</M>");
    checkInterpretation ("1024a", "<#>1024</#><M>a</M>");
  }
}

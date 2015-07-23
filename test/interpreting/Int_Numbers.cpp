/**
 * @file Int_Numbers.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"

namespace {
  MathDocument executeTest (const std::string &input, const std::string &expected)
  {
    MathDocument doc;
    std::string outputString;

    interpretToDocument (input, doc, outputString);
    CHECK (outputString == (expected + "<eol>"));
    return doc;
  }
}

// =========================================================================
TEST_CASE("interpret/Numbers/whole", "[interpret][Numbers]") {
  SECTION("simple") {
    executeTest ("1", "<#>1</#>");
    executeTest ("10", "<#>10</#>");
    executeTest ("1048576", "<#>1048576</#>");
  }
  SECTION("with commas") {
    executeTest ("1,024", "<#>1,024</#>");
    executeTest ("1,048,576", "<#>1,048,576</#>");
    executeTest ("10,048,576", "<#>10,048,576</#>");
    executeTest ("100,048,576", "<#>100,048,576</#>");
  }
  SECTION("with spaces") {
    executeTest ("1 024", "<#>1 024</#>");
    executeTest ("1 048 576", "<#>1 048 576</#>");
    executeTest ("10 048 576", "<#>10 048 576</#>");
    executeTest ("100 048 576", "<#>100 048 576</#>");
  }
}

// =========================================================================
TEST_CASE("interpret/Numbers/decimal", "[interpret][Numbers]") {
  SECTION("simple") {
    executeTest ("1.1", "<#>1.1</#>");
    executeTest ("10.25", "<#>10.25</#>");
    executeTest ("1048576.171274174", "<#>1048576.171274174</#>");
  }
  SECTION("with commas") {
    executeTest ("1,024.25", "<#>1,024.25</#>");
    executeTest ("1,048,576.3", "<#>1,048,576.3</#>");
    executeTest ("10,048,576.224 116", "<#>10,048,576.224 116</#>");
    executeTest ("100,048,576.72", "<#>100,048,576.72</#>");
    executeTest ("12,248,57", "<#>12,248</#><COMMA><#>57</#>");
  }
  SECTION("with spaces") {
    executeTest ("1 024.25", "<#>1 024.25</#>");
    executeTest ("1 048 576.3", "<#>1 048 576.3</#>");
    executeTest ("10 048 576.224 116", "<#>10 048 576.224 116</#>");
    executeTest ("100 048 576.72", "<#>100 048 576.72</#>");
  }
}


// =========================================================================
TEST_CASE("interpret/Numbers/terminations", "[interpret][Numbers]") {
  SECTION("forced terminations (lhs)") {
    executeTest ("101 102 103", "<#>101 102 103</#>");
    executeTest ("101\\ 102 103", "<#>101</#><#>102 103</#>");
    executeTest ("101 102\\ 103", "<#>101 102</#><#>103</#>");
  }
  SECTION("forced terminations (rhs)") {
    executeTest ("0.101 102 103", "<#>0.101 102 103</#>");
    executeTest ("0.101\\ 102 103", "<#>0.101</#><#>102 103</#>");
    executeTest ("0.101 102\\ 103", "<#>0.101 102</#><#>103</#>");
  }
  SECTION("terminating on periods") {
    executeTest ("1024.", "<#>1024</#><PERIOD>");
    executeTest ("1024.x", "<#>1024</#><PERIOD><M>x</M>");
  }
  SECTION("terminating on spaces, letters, etc.") {
    executeTest ("1024 = x", "<#>1024</#><equals><M>x</M>");
    executeTest ("1024a", "<#>1024</#><M>a</M>");
  }
}

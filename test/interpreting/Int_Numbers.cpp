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
    CHECK (expected == outputString);
    return doc;
  }
}

// =========================================================================
TEST_CASE("interpret/Numbers/whole", "[interpret][Numbers]") {
  SECTION("simple") {
    executeTest ("1", "<#>1</#><eol>");
    executeTest ("10", "<#>10</#><eol>");
    executeTest ("1048576", "<#>1048576</#><eol>");
  }
  SECTION("with commas") {
    executeTest ("1,024", "<#>1,024</#><eol>");
    executeTest ("1,048,576", "<#>1,048,576</#><eol>");
    executeTest ("10,048,576", "<#>10,048,576</#><eol>");
    executeTest ("100,048,576", "<#>100,048,576</#><eol>");
  }
  SECTION("with spaces") {
    executeTest ("1 024", "<#>1 024</#><eol>");
    executeTest ("1 048 576", "<#>1 048 576</#><eol>");
    executeTest ("10 048 576", "<#>10 048 576</#><eol>");
    executeTest ("100 048 576", "<#>100 048 576</#><eol>");
  }
}

// =========================================================================
TEST_CASE("interpret/Numbers/decimal", "[interpret][Numbers]") {
  SECTION("simple") {
    executeTest ("1.1", "<#>1.1</#><eol>");
    executeTest ("10.25", "<#>10.25</#><eol>");
    executeTest ("1048576.171274174", "<#>1048576.171274174</#><eol>");
  }
  SECTION("with commas") {
    executeTest ("1,024.25", "<#>1,024.25</#><eol>");
    executeTest ("1,048,576.3", "<#>1,048,576.3</#><eol>");
    executeTest ("10,048,576.224 116", "<#>10,048,576.224 116</#><eol>");
    executeTest ("100,048,576.72", "<#>100,048,576.72</#><eol>");
    executeTest ("12,248,57", "<#>12,248</#><,><#>57</#><eol>");
  }
  SECTION("with spaces") {
    executeTest ("1 024.25", "<#>1 024.25</#><eol>");
    executeTest ("1 048 576.3", "<#>1 048 576.3</#><eol>");
    executeTest ("10 048 576.224 116", "<#>10 048 576.224 116</#><eol>");
    executeTest ("100 048 576.72", "<#>100 048 576.72</#><eol>");
  }
}


// =========================================================================
TEST_CASE("interpret/Numbers/terminations", "[interpret][Numbers]") {
  SECTION("forced terminations (lhs)") {
    executeTest ("101 102 103", "<#>101 102 103</#><eol>");
    executeTest ("101\\ 102 103", "<#>101</#><#>102 103</#><eol>");
    executeTest ("101 102\\ 103", "<#>101 102</#><#>103</#><eol>");
  }
  SECTION("forced terminations (rhs)") {
    executeTest ("0.101 102 103", "<#>0.101 102 103</#><eol>");
    executeTest ("0.101\\ 102 103", "<#>0.101</#><#>102 103</#><eol>");
    executeTest ("0.101 102\\ 103", "<#>0.101 102</#><#>103</#><eol>");
  }
}

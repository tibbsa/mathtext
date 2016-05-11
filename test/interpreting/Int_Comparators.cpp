/**
 * @file Int_Comparators.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "mttest.h"

// =========================================================================
TEST_CASE("interpret/Comparators", "[interpret][Comparators]") {
  checkInterpretation ("<", "<is less than>");
  checkInterpretation (">", "<is greater than>");
  checkInterpretation ("<=", "<is less than or equal to>");
  checkInterpretation (">=", "<is greater than or equal to>");
  checkInterpretation ("=", "<equals>");
  checkInterpretation ("~=", "<approximately equals>");
  checkInterpretation ("!=", "<is not equal to>");
}


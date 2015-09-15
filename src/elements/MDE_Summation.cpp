/**
 * @file MDE_Summation.cpp
 * Summation (sigma) sign (with a lower bound, upper bound), typically followed
 * by a summation formula of some descriptino.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Summation::MDE_Summation(const MDEVector lower, const MDEVector upper) :
  lowerBound(lower), upperBound(upper)
{
}

MDEVector MDE_Summation::getLowerBound (void) const
{
  return lowerBound;
}

MDEVector MDE_Summation::getUpperBound (void) const
{
  return upperBound;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Summation::getString (void) const
{
  std::string output;

  output = "<SUM";

  if (lowerBound.size() > 0) {
    output += " LOWER:\"";
    BOOST_FOREACH (const MathDocumentElementPtr &ePtr, lowerBound) {
      output += ePtr.get()->getString();
    }
    output += "\"";
  }
  if (upperBound.size() > 0) {
    output += " UPPER:\"";
    BOOST_FOREACH (const MathDocumentElementPtr &ePtr, upperBound) {
      output += ePtr.get()->getString();
    }
    output += "\"";
  }

  output += ">";
  return output;
}



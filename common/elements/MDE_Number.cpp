/**
 * @file MDE_Number.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Number::MDE_Number(const MDE_Number::NumberType type,
		       const std::string &whole,
		       const std::string &decimals)
  : numType(type), lhs(whole), rhs(decimals)
{
}


std::string MDE_Number::getWholePortion (void) const
{
  return lhs;
}

std::string MDE_Number::getDecimalPortion (void) const
{
  return rhs;
}

std::string MDE_Number::getStandardNotation (void) const
{
  if (!rhs.empty())
    return std::string((isNegative() ? "-" : "") + getWholePortion() + "." + getDecimalPortion());
  else
    return std::string((isNegative() ? "-" : "") + getWholePortion());
}

bool MDE_Number::isNegative (void) const
{
  return (numType == NEGATIVE);
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Number::getString (void) const
{
  std::string output;

  output = "<#>" + getStandardNotation() + "</#>";

  return output;
}


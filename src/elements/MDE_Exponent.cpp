/**
 * @file MDE_Exponent.cpp
 * An exponent
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Exponent::MDE_Exponent(const MDEVector exponent) : contents(exponent)
{
}

MDEVector MDE_Exponent::getValue (void) const
{
  return contents;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Exponent::getString (void) const
{
  std::string output;

  output = "<EXP>";
  for (MDEVector::const_iterator it = contents.begin();
       it != contents.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }
  output += "</EXP>";
  return output;
}



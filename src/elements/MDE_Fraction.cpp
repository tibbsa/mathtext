/**
 * @file MDE_Fraction.cpp
 * A fraction, with a numerator and a denominator component
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Fraction::MDE_Fraction(const MDEVector num, const MDEVector den) :
  numerator(num), denominator(den)
{
}

MDEVector MDE_Fraction::getNumerator (void) const
{
  return numerator;
}

MDEVector MDE_Fraction::getDenominator (void) const
{
  return denominator;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Fraction::getString (void) const
{
  std::string output;

  output = "<FRAC>";
  for (MDEVector::const_iterator it = numerator.begin();
       it != numerator.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }
  output += "<OVER>";

  for (MDEVector::const_iterator it = denominator.begin();
       it != denominator.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }

  output += "</FRAC>";
  return output;
}



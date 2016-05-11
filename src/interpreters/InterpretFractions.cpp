/**
 * @file InterpretFractions.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include <boost/format.hpp>

#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a fraction: @num~den#

 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty numerator/denominator
 */
bool MathInterpreter::interpretFraction (MDEVector &target,
				      const std::string &src,
				      size_t &i)
{
  if (src [i] == '#' || src [i] == '~') {
    MSG_WARNING(SUSPECT_FRACTION, boost::str(boost::format("found '%c' modifier outside of a fraction") % src [i]));
    return false;
  }

  if (src [i] != '@')
    return false;

  // Count forward to find the "end fraction" indicator (#). Allow
  // nested fractions but do not evaluate them as we go for now --
  // just try to get the outermost fraction that we see.
  int num_nested_fractions = 0;
  bool foundTerminator = false;
  bool foundDividingLine = false;
  size_t pos;
  std::string numerator;
  std::string denominator;
  for (pos = i+1; pos < src.length(); pos++) {
    // Skip escaped characters or "approximately equal to" modifiers
    // that appear within a fraction.
    if (src.substr (pos, 2) == "\\#" || src.substr(pos, 2) == "\\@"
	|| src.substr (pos, 2) == "\\~" || src.substr(pos, 2) == "~=")
      pos += 2;

    // Look for nested fractions
    if (src [pos] == '@')
      num_nested_fractions++;
    else // Look for the "end fraction" indicators
    if (src [pos] == '#') {
      if (!num_nested_fractions) {
	foundTerminator = true;
	break;
      }
      else
	num_nested_fractions--;
    }
    else // Look for the dividing line if we aren't in a nested fraction
    if (!foundDividingLine && !num_nested_fractions && src [pos] == '~') {
      foundDividingLine = true;
      continue;
    }

    // Add this character  to the numerator or denominator as requierd
    if (!foundDividingLine)
      numerator += src [pos];
    else
      denominator += src [pos];
  }

  if (!foundTerminator) {
    MSG_ERROR(FRACTION_NOT_TERMINATED, boost::str(boost::format("end of line encountered while still inside %d fraction%s") % (num_nested_fractions+1) % (num_nested_fractions >= 2 ? "s" : "")));

    BOOST_THROW_EXCEPTION (MathInterpreterException());
  }

  // advance cursor
  i = pos + 1;

  LOG_TRACE << "* found fraction: " << numerator << " // " << denominator;

  MDEVector numvec;
  MDEVector denvec;

  numvec = interpretBuffer (numerator);
  denvec = interpretBuffer (denominator);

  target.push_back (boost::make_shared<MDE_Fraction>(numvec, denvec));
  return true;
}

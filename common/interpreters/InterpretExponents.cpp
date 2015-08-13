/**
 * @file InterpretExponents.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret an exponent: ^item or ^(item)
 *
 * An "item" is everything up to the next space, sign of operation,
 * sign of comparison, etc.
 *
 * If ^ is followed by parenthesees then the entire paranthesees is
 * taken as the exponent.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty exponents: x^() for example
 */
bool MathInterpreter::interpretExponent (MDEVector &target,
				      const std::string &src,
				      size_t &i)
{
  if (src [i] != '^')
    return false;

  std::string exponent_contents;

  // If this is followed by an open paren '(', read until the closing
  // paren
  i++;
  if (src [i] == '(') {
    if (!extractGroup(exponent_contents, src, i)) {
      MSG_ERROR(EXPONENT_NOT_TERMINATED, boost::str(boost::format("text in exponent: '%s'") % exponent_contents));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else if (src [i] == '@') {
    if (!extractGroup(exponent_contents, src, i, "@", "#", true)) {
      MSG_ERROR(EXPONENT_NOT_TERMINATED, boost::str(boost::format("text in exponent: '%s'") % exponent_contents));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else {
    extractItem(exponent_contents, src, i);
  }

  LOG_TRACE << "* found exponent: " << exponent_contents;

  MDEVector v;
  v = interpretBuffer (exponent_contents);
  target.push_back (boost::make_shared<MDE_Exponent>(v));
  return true;
}

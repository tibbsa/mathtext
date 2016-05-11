/**
 * @file InterpretOperators.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "logging.h"

#include "MathInterpreter.h"

/**
 * Attempts to interpret the specified characters as signs of operation
 * (+, -, / *)
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
bool MathInterpreter::interpretOperator (MDEVector &target,
					 const std::string &src,
					 size_t &i)
{
  const std::string temp = src.substr(i, 3);

  if (temp != " / " && !isOneOf(temp[0], "+*-"))
    return false;

  // +/- is a different symbol and ought not be handled here
  if (temp == "+/-" || temp == "-/+")
    return false;

  if (temp[0] == '+') {
    LOG_TRACE << "* adding addition sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::ADDITION));
    i++;
  } else if (temp[0] == '*') {
    LOG_TRACE << "* adding multiplication sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::MULTIPLICATION));
    i++;
  } else if (temp[0] == '-') {
    // We might have to make a decision here as to whether what follows is a
    // "negative number" or whether what we are seeing is a minus sign.
    //
    // ##TODO: Consider how to handle this.  It might matter in braille.
    //         but the logic below doesn't work. x^-1 = x9"-#A (with a true
    //         minus sign on it), not just a negative number.

    /*
    //
    // If we see a number immediately after the minus sign, interpret that
    // as a negative number instead.
    //
    if (temp.length() > 1 && isdigit(temp[1])) {
      LOG_TRACE << "- subtraction sign might be a negative number instead";
      if (interpretNumber(target, src, i)) {
	// got added as a number; do nothing
	return true;
      }
    }
    */

    LOG_TRACE << "* adding subtraction sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::SUBTRACTION));
    i++;
  }
  else if (temp == " / ") {
    LOG_TRACE << "* adding division sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::DIVISION));
    i += 3;
  }
  else
    assert (false); // Tests above should prevent us from getting here

  // skip whitespace following an operator
  while (i < src.length() && isspace(src[i]))
    i++;

  return true;
}

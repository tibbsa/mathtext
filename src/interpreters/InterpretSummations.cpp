/**
 * @file InterpretSummations.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a fraction: `S(lower, upper) ...

 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 */
bool MathInterpreter::interpretSummation (MDEVector &target,
					  const std::string &src,
					  size_t &i)
{
  size_t pos = i;

  if (src.substr(pos, 2) != "`S" || (src.length() - pos) < 4)
    return false;

  pos += 2;
  // `S should be followed by a space or a ( -- if not, bail o ut
  while (pos < src.length() && isspace(src[pos]))
    pos++;

  if (src[pos] != '(')
    return false;

  // skip over opening parantheses
  pos++;

  std::string lowerBoundStr;
  std::string upperBoundStr;
  LOG_TRACE << "before ETD: pos " << pos << ", buffer: " << src.substr(pos, 20);
  if (!extractToDelimiter(lowerBoundStr, src, pos, ",")) {
    //## TODO: ADD ERROR MESSAGE - unable to extrtact lower bound
    BOOST_THROW_EXCEPTION (MathInterpreterException());
  }
  LOG_TRACE << "after ETD: pos " << pos << ", buffer: " << src.substr(pos, 20);

  // skip spaces
  while (pos < src.length() && isspace(src[pos]))
    pos++;

  LOG_TRACE << "retrieved lower bound: " << lowerBoundStr;
  LOG_TRACE << "extractToDelimeter calling on pos " << pos << ": buffer '" << src.substr(pos, 20) << "...";

  if (!extractToDelimiter(upperBoundStr, src, pos, ")")) {
    //## TODO: ADD ERROR MESSAGE - unable to extrtact upper bound
    BOOST_THROW_EXCEPTION (MathInterpreterException());
  }

  // advance cursor
  i = pos + 1;

  LOG_TRACE << "* found summation: lower=" << lowerBoundStr << ", upper=" << upperBoundStr;

  MDEVector lowerBoundVec;
  MDEVector upperBoundVec;

  lowerBoundVec = interpretBuffer (lowerBoundStr);
  upperBoundVec = interpretBuffer (upperBoundStr);

  target.push_back (boost::make_shared<MDE_Summation>(lowerBoundVec, upperBoundVec));
  return true;
}

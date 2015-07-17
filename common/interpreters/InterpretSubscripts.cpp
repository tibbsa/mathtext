/**
 * @file InterpretSubscripts.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a subscript: _item or _(item)
 *
 * An "item" is everything up to the next space, sign of operation,
 * sign of comparison, etc.
 *
 * If _ is followed by parenthesees then the entire paranthesees is 
 * taken as the subscript.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty subscriptions: x_() for example
 */
bool MathInterpreter::interpretSubscript (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  if (src [i] != '_') 
    return false;

  std::string subscript_contents;


  // If this is followed by an open paren '(', read until the closing 
  // paren
  i++;
  if (src [i] == '(') {
    if (!extractGroup(subscript_contents, src, i)) {
      MSG_ERROR(SUBSCRIPT_NOT_TERMINATED, boost::str(boost::format("text in subscript: '%s'") % subscript_contents));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else if (src [i] == '@') {
    if (!extractGroup(subscript_contents, src, i, "@", "#", true)) {
      MSG_ERROR(EXPONENT_NOT_TERMINATED, boost::str(boost::format("text in fractional subscript: '%s'") % subscript_contents));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else {
    extractItem(subscript_contents, src, i);
  }

  LOG_TRACE << "* found subscript: " << subscript_contents;

  MDEVector v;
  v = interpretBuffer (subscript_contents);
  target.push_back (boost::make_shared<MDE_Subscript>(v));
  return true;
}

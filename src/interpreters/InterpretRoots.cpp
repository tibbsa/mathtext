/**
 * @file InterpretRoots.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a root: _/100 (square root of 100)
 *                            or _/(n+1) (square root of 'n+1')
 *                            or _/3(16) (cube root of 16)
 *                            or _/[n^2](x + 2y + 3c^2) (complex root index)
 *
 * For the basic form, the root is everything up to the next space, sign of
 * operation, sign of comparison, etc.
 *
 * If _/ is followed by parenthesees then the entire paranthesees is
 * taken as the root argument.
 *
 * If _/ is followed by a number, and immediately followed by an expression
 * in paranthesees, then the number is considered the root index and the
 * entire paranthesees is taken as the root argment.
 *
 * If _/ is followed by a square bracket, then the entire square bracket is
 * taken as the root index, and the entire paranthesees following that is
 * taken as the root argument.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
bool MathInterpreter::interpretRoot (MDEVector &target,
				  const std::string &src,
				  size_t &i)
{
  if (src.substr(i, 2) != "_/")
    return false;

  std::string root_index;
  std::string root_argument;
  MDEVector root_index_vector;

  i += 2;

  // Look for root indexes, which may be a simple letter or number, or a
  // more complex expression bounded by square brackets.
  boost::regex root_index_regex("(\\w)+\\(");
  boost::smatch mr;

  std::string searchStr = src.substr(i);
  if (boost::regex_search(searchStr, mr, root_index_regex)) {
    root_index = mr[1];
    i += root_index.length();
  } else if (src [i] == '[') {
    if (!extractGroup(root_index, src, i, "[", "]")) {
      MSG_ERROR(ROOT_INDEX_NOT_TERMINATED, boost::str(boost::format("text in root index: '%s'") % root_index));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  }

  if (!root_index.empty()) {
    LOG_TRACE << "- found a root with an index: generating index vector for '" << root_index << "'";
    root_index_vector = interpretBuffer(root_index);
  }
  else
    LOG_TRACE << "- found a root with no index";

  // If this is followed by an open paren '(', read until the closing
  // paren; otherwise, read until the end of the item

  if (src [i] == '@') {
    if (!extractGroup(root_argument, src, i, "@", "#", true)) {
      MSG_ERROR(ROOT_NOT_TERMINATED, boost::str(boost::format("text in fractional root: '%s'") % root_argument));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else if (src [i] == '(') {
    if (!extractGroup(root_argument, src, i)) {
      MSG_ERROR(ROOT_NOT_TERMINATED, boost::str(boost::format("text in root: '%s'") % root_argument));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }
  } else {
    extractItem(root_argument, src, i);
  }

  LOG_TRACE << "* found root: index='" << root_index << "', argument: '" << root_argument << "'";

  MDEVector root_argument_vector = interpretBuffer (root_argument);
  target.push_back (boost::make_shared<MDE_Root>(root_index_vector, root_argument_vector));
  return true;
}

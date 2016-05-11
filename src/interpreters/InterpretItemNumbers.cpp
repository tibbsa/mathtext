/**
 * @file InterpretItemNumbers.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/assign.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "logging.h"

#include "MathInterpreter.h"

/**
 * Attempts to interpret the specified characters as item numbers. THese
 * may appear at the start of the line.  We treat them as a special text
 * block to allow rendering engines to distinguish question numbers as
 * they see fit.
 *
 * Accepted formats:
 * "1. " "a. "
 * "1) " "a) "
 * "(1) " "(a) "
 */
bool MathInterpreter::interpretItemNumber (MDEVector &target,
					const std::string &src,
					size_t &i)
{
  const std::vector<std::string> patterns = boost::assign::list_of
    ( std::string("^(\\d{1,4}\\.\\s+)") ) // 1.
    ( std::string("^([A-Za-z]{1}\\.\\s+)") ) // a.
    ( std::string("^(\\d{1,4}\\)\\s+)") ) // 1)
    ( std::string("^([A-Za-z]{1}\\)\\s+)") ) // a)
    ( std::string("^(\\(\\d{1,4}\\)\\s+)") ) // (1)
    ( std::string("^(\\([A-Za-z]{1}\\)\\s+)") ) // (a)
    ;

  for (std::vector<std::string>::const_iterator it = patterns.begin();
       it != patterns.end(); ++it) {
    const std::string pattern = *it;

    boost::regex exp(pattern);
    boost::smatch mr;

    // search for item numbers - assume they will not be longer than 10 chars
    // (123456) 
    std::string searchStr = src.substr(i, 10);
    if (boost::regex_search(searchStr, mr, exp)) {
      std::string item_number (mr[1].first, mr[1].second);
      boost::trim(item_number);

      LOG_TRACE << "* added item number '" << item_number << "' with pattern '" << pattern << "'";
      i = i + mr.length();
      target.push_back (boost::make_shared<MDE_ItemNumber>(item_number));
      return true;
    }
  }

  return false;
}

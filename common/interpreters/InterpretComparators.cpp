/**
 * @file InterpretComparators.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"

/**
 * Attempts to interpret the specified characters as signs of comparison
 * (< > = != ~= <= >=)
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
namespace { 
  struct ComparatorSymbolMap { 
    ComparatorSymbolMap (const std::string &s, const MDE_Comparator::Comparator c) : search(s), symbol(c) {}
    std::string search;
    MDE_Comparator::Comparator symbol;
  };
}

bool MathInterpreter::interpretComparator (MDEVector &target,
					const std::string &src, 
					size_t &i)
{
  static const std::vector<ComparatorSymbolMap> map = boost::assign::list_of 
    ( ComparatorSymbolMap("<=", MDE_Comparator::LESS_THAN_EQUALS) )
    ( ComparatorSymbolMap(">=", MDE_Comparator::GREATER_THAN_EQUALS) )
    ( ComparatorSymbolMap("!=", MDE_Comparator::NOT_EQUALS) )
    ( ComparatorSymbolMap("~=", MDE_Comparator::APPROX_EQUALS) )
    ( ComparatorSymbolMap("<", MDE_Comparator::LESS_THAN) )
    ( ComparatorSymbolMap(">", MDE_Comparator::GREATER_THAN) )
    ( ComparatorSymbolMap("=", MDE_Comparator::EQUALS) );

  const std::string temp = src.substr(i, 2);

  for (std::vector<ComparatorSymbolMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const ComparatorSymbolMap &mi = *it;
    // Look for the first character
    if (temp[0] == mi.search[0]) {
      // It matches. If the second one also has to match, consider it too.
      if (mi.search.length() == 1 || temp[1] == mi.search[1]) {
	LOG_TRACE << "* added comparator sign (" << mi.search << ")";
	target.push_back (boost::make_shared<MDE_Comparator>(mi.symbol));
	i += mi.search.length();

	// Skip whitespace after a sign of comparison
	while (i < src.length() && isspace(src[i]))
	  i++;

	return true;
      }
    }
  }

  return false;
}

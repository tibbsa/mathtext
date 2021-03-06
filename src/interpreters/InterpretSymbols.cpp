/**
 * @file InterpretSymbols.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/assign.hpp>

#include "logging.h"

#include "MathInterpreter.h"
#include "MathInterpreterMsg.h"

/**
 * Attempts to interpret a variety of random symbols.

 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
namespace {
  struct SymbolMap {
    SymbolMap (const std::string &s, const MDE_Symbol::Symbol sym) : search(s), symbol(sym) {}
    std::string search;
    MDE_Symbol::Symbol symbol;
  };
}

bool MathInterpreter::interpretSymbol (MDEVector &target,
				    const std::string &src,
				    size_t &i)
{
  #define MAX_SYMBOL_LEN 2
  // Be sure to update MAX_SYMBOL_LEN if longer symbols get added to
  // this table.  Place longer symbols at the top.
  static const std::vector<SymbolMap> map = boost::assign::list_of
    ( SymbolMap("%%", MDE_Symbol::PERCENT) )
    ( SymbolMap("/\\", MDE_Symbol::THEREFORE) )
    ( SymbolMap(",", MDE_Symbol::COMMA) )
    ( SymbolMap("`C", MDE_Symbol::CURRENCY_CENTS) )
    ( SymbolMap("`E", MDE_Symbol::CURRENCY_EURO) )
    ( SymbolMap("`F", MDE_Symbol::CURRENCY_FRANC) )
    ( SymbolMap("`P", MDE_Symbol::CURRENCY_POUND) )
    ( SymbolMap("`$", MDE_Symbol::CURRENCY_DOLLAR) )
    ( SymbolMap("`Y", MDE_Symbol::CURRENCY_YEN) )
    ( SymbolMap("{", MDE_Symbol::LEFT_BRACE) )
    ( SymbolMap("[", MDE_Symbol::LEFT_BRACKET) )
    ( SymbolMap("(", MDE_Symbol::LEFT_PAREN) )
    ( SymbolMap("!", MDE_Symbol::FACTORIAL) )
    ( SymbolMap(".", MDE_Symbol::PERIOD) )
    ( SymbolMap("}", MDE_Symbol::RIGHT_BRACE) )
    ( SymbolMap("]", MDE_Symbol::RIGHT_BRACKET) )
    ( SymbolMap(")", MDE_Symbol::RIGHT_PAREN) )
    ;

  const std::string temp = src.substr(i, MAX_SYMBOL_LEN);

  for (std::vector<SymbolMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const SymbolMap &mi = *it;

    if (strBeginsWith(temp, mi.search)) {
      LOG_TRACE << "* added symbol (" << mi.search << ")";
      target.push_back (boost::make_shared<MDE_Symbol>(mi.symbol));
      i += mi.search.length();
      return true;
    }
  }

  return false;
}

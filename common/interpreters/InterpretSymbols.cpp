/**
 * @file InterpretSymbols.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"

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
    ( SymbolMap("{", MDE_Symbol::LEFT_BRACE) )
    ( SymbolMap("[", MDE_Symbol::LEFT_BRACKET) )
    ( SymbolMap("(", MDE_Symbol::LEFT_PAREN) )
    ( SymbolMap("!", MDE_Symbol::FACTORIAL) )
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

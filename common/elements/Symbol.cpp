/**
 * @file MDE_Symbol.cpp
 * Various symbols that do nto require particularly special handling, do 
 * not have attached 'arguments', etc. 
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

#include <map>

#include <boost/assign.hpp>

#include "logging.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Symbol::MDE_Symbol(const MDE_Symbol::Symbol s) : symbol(s)
{
}

MDE_Symbol::Symbol MDE_Symbol::getSymbol (void) const 
{
  return symbol;
}

void MDE_Symbol::setSymbol (const MDE_Symbol::Symbol s)
{
  symbol = s;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Symbol::getString (void) const
{
  static std::map<MDE_Symbol::Symbol,std::string> map = boost::assign::map_list_of
    (COMMA, ",")
    (FACTORIAL, "!")
    (LEFT_BRACE, "{")
    (LEFT_BRACKET, "[")
    (LEFT_PAREN, "(")
    (PERCENT, "%")
    (RIGHT_BRACE, "}")
    (RIGHT_BRACKET, "]")
    (RIGHT_PAREN, ")")
    (THEREFORE, "therefore")
    ;

  assert (map.count(symbol) == 1);

  return std::string("<" + map[symbol] + ">");
}



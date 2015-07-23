/**
 * @file MDE_Symbol.cpp
 * Various symbols that do nto require particularly special handling, do 
 * not have attached 'arguments', etc. 
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
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
#define MAP(m) (m, #m)
  static std::map<MDE_Symbol::Symbol,std::string> map = boost::assign::map_list_of
    MAP(COMMA)
    MAP(CURRENCY_CENTS)
    MAP(CURRENCY_EURO)
    MAP(CURRENCY_FRANC)
    MAP(CURRENCY_POUND)
    MAP(CURRENCY_DOLLAR)
    MAP(CURRENCY_YEN)
    MAP(FACTORIAL)
    MAP(LEFT_BRACE)
    MAP(LEFT_BRACKET)
    MAP(LEFT_PAREN)
    MAP(PERCENT)
    MAP(PERIOD)
    MAP(RIGHT_BRACE)
    MAP(RIGHT_BRACKET)
    MAP(RIGHT_PAREN)
    MAP(THEREFORE)
    ;
#undef MAP

  assert (map.count(symbol) == 1);
  return std::string("<" + map[symbol] + ">");
}



/**
 * @file MDE_Symbol.cpp
 * Various symbols that do nto require particularly special handling, do 
 * not have attached 'arguments', etc. 
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

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
  std::string output;

  if (symbol == COMMA)
    output = "<,>";
  else if (symbol == FACTORIAL)
    output = "<!>";
  else if (symbol == PERCENT)
    output = "<%>";
  else if (symbol == THEREFORE)
    output = "<therefore>";
  else
    assert (false);

  return output;
}



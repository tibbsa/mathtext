/**
 * @file MDE_Subscript.cpp
 * A subscript
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "logging.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Subscript::MDE_Subscript(const MDEVector subscript) : contents(subscript)
{
}

MDEVector MDE_Subscript::getValue (void) const 
{
  return contents;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Subscript::getString (void) const
{
  std::string output;

  output = "<SUB>";
  for (MDEVector::const_iterator it = contents.begin();
       it != contents.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }
  output += "</SUB>";
  return output;
}



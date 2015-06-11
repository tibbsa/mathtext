/**
 * @file MDE.cpp
 * Implementation of the MathDocumentElement abstract class.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "logging.h"
#include "MathDocumentElements.h"


/* ========================= PUBLIC FUNCTION =============================== */
/**
 * Sets up a new, blank math element
 */
MathDocumentElement::MathDocumentElement()
{
}

/**
 * Destructor
 */
MathDocumentElement::~MathDocumentElement()
{
}


/**
 * Generate a string representation of this element (primarily for 
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MathDocumentElement::getString (void) const
{
  std::string output;

  /*
  if(exponent)
    output += "^(" + exponent->getString() + ")";
  if(subscript)
    output += "_(" + subscript->getString() + ")";
  */

  return output;
}


std::ostream &operator<<(std::ostream &os, const MathDocumentElement &e){
  return os << e.getString();
}


/**
 * @file MDE_MathBlock.cpp
 * "Generic" math content block (unprocessed)
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
MDE_MathBlock::MDE_MathBlock() : MDE_GenericText()
{
}

MDE_MathBlock::MDE_MathBlock(const std::string &contents) : 
  MDE_GenericText(contents)
{
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_MathBlock::getString (void) const
{
  std::string output;

  output = "<M>" + payload + MathDocumentElement::getString() + "</M>";

  return output;
}



/**
 * @file MDE_TextBlock.cpp
 * "Generic" text content block (unprocessed)
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
MDE_TextBlock::MDE_TextBlock() : MDE_GenericText()
{
}

MDE_TextBlock::MDE_TextBlock(const std::string &contents) : 
  MDE_GenericText(contents)
{
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_TextBlock::getString (void) const
{
  std::string output;

  output = "<T>" + payload + MathDocumentElement::getString() + "</T>";

  return output;
}

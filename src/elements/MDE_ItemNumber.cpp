/**
 * @file MDE_ItemNumber.cpp
 * A special kind of text block that represents an "item number" (as might
 * appear in homework) at the start of a line.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_ItemNumber::MDE_ItemNumber(const std::string &contents) :
  MDE_TextBlock(contents)
{
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_ItemNumber::getString (void) const
{
  std::string output;

  output = "<ITEMNO>" + payload + "</ITEMNO>";

  return output;
}



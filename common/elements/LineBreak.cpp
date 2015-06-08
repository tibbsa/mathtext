/**
 * @file LineBreak.cpp
 * Used to indcate the end of a line or statement
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

std::string MDE_LineBreak::getString (void) const
{
  return std::string("<eol>");
}

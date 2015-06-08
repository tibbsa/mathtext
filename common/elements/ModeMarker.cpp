/**
 * @file MDE_ModeMarker.cpp
 * Mode markers are used to indicate whether the following material is 
 * primarily mathematical in nature, or primarily textual in nature.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

std::string MDE_MathModeMarker::getString (void) const
{
  return std::string("$$");
}

std::string MDE_TextModeMarker::getString (void) const
{
  return std::string("&&");
}

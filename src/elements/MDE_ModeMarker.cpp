/**
 * @file MDE_ModeMarker.cpp
 * Mode markers are used to indicate whether the following material is
 * primarily mathematical in nature, or primarily textual in nature.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

MDE_MathModeMarker::MDE_MathModeMarker (const MDE_MathModeMarker::MarkerType mt) : type(mt)
{
}

MDE_MathModeMarker::MarkerType MDE_MathModeMarker::getType (void) const
{
  return type;
}

std::string MDE_MathModeMarker::getString (void) const
{
  if (type == MDE_MathModeMarker::BLOCK_MARKER)
    return std::string("<$$>");
  else
    return std::string("<$>");
}


MDE_TextModeMarker::MDE_TextModeMarker (const MDE_TextModeMarker::MarkerType mt) : type(mt)
{
}

MDE_TextModeMarker::MarkerType MDE_TextModeMarker::getType (void) const
{
  return type;
}

std::string MDE_TextModeMarker::getString (void) const
{
  if (type == MDE_TextModeMarker::BLOCK_MARKER)
    return std::string("<&&>");
  else
    return std::string("<&>");
}

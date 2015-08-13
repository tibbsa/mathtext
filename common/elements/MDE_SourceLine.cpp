/**
 * @file MDE_SourceLine.cpp
 * Contains a complete copy of the original MathText source lines
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_SourceLine::MDE_SourceLine(const std::string &fname,
			       const unsigned long line_start,
			       const unsigned long line_end,
			       const std::string &contents)
  : filename(fname), line1(line_start), line2(line_end), payload(contents)
{

}

std::string MDE_SourceLine::getFilename (void) const
{
  return filename;
}

unsigned long MDE_SourceLine::getStartLine (void) const
{
  return line1;
}

unsigned long MDE_SourceLine::getEndLine (void) const
{
  return line2;
}

std::string MDE_SourceLine::getContents (void) const
{
  return payload;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_SourceLine::getString (void) const
{
  if (line1 == line2)
    return boost::str(boost::format("<SOURCE %s:%lu>%s</SOURCE>") % filename % line1 % payload);
  else
    return boost::str(boost::format("<SOURCE %s:%lu-%lu>%s</SOURCE>") % filename % line1 % line2 % payload);
}

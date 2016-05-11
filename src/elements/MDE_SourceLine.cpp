/**
 * @file 
 * Contains a complete copy of the original MathText source lines
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/format.hpp>
#include "MathDocumentElements.h"

/**
 * Constructor, setting up the object.
 *
 * @param [in] fname Path/filename from which this line of text was loaded
 * @param [in] line_start Line number where this expression began
 * @param [in] line_end Last line number where this expression concluded
 * @param [in] contents Text as loaded from the file
 */
MDE_SourceLine::MDE_SourceLine(const std::string &fname,
			       const unsigned long line_start,
			       const unsigned long line_end,
			       const std::string &contents)
  : filename(fname), line1(line_start), line2(line_end), payload(contents)
{
}

/**
 * Returns the name of the file from which this line of text was loaded.
 *
 * @return filename from which this line of text was loaded
 */
std::string MDE_SourceLine::getFilename (void) const
{
  return filename;
}

/**
* Returns the line number where this line of text began
*
* @return first line number where this text appeared
*/
unsigned long MDE_SourceLine::getStartLine (void) const
{
  return line1;
}

/**
* Returns the line number where this line of text finished
*
* @return last line number where this text appeared
*/
unsigned long MDE_SourceLine::getEndLine(void) const
{
  return line2;
}

/**
* Returns the contents of this line of text.
*
* @return Text as loaded from the file.
*/
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

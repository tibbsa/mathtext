/**
 * @file MathDocument.cpp
 * Implementation of the container holding one line of the raw source document
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <iostream>
#include <sstream>
#include "MathDocumentLine.h"

/**
 * Sets up a new math document line object.
 *
 * @param source_filename Filename of document line loaded from
 * @param source_linenumber1 Number of line in file
 * @param source_linenumber2 Number of line (if continuation line) in file
 * @param text Contents of the line
 */
MathDocumentLine::MathDocumentLine (const std::string &source_filename,
				    const unsigned long source_linenumber1,
				    const unsigned long source_linenumber2,
				    const std::string &text) : m_filename(source_filename), m_line1(source_linenumber1), m_line2(source_linenumber2), m_content(text)
{
}

/**
 * Retrieve the filename associated with this line.
 *
 * @return Filename associated with this line
 */
std::string MathDocumentLine::getFilename (void) const
{
  return m_filename;
}

/**
 * Retrieve the starting line number associated with this line.
 *
 * @return First source line number associated with this line
 */
unsigned long MathDocumentLine::getStartLineNumber (void) const
{
  return m_line1;
}

/**
 * Retrieve the ending line number associated with this line.
 *
 * @return Final source line numer associated with this line
 */
unsigned long MathDocumentLine::getEndLineNumber (void) const
{
  return m_line2;
}

/**
 * Retrieve the contents of the line.
 *
 * @return Contents of the line.
 */
std::string MathDocumentLine::getContent (void) const
{
  return m_content;
}

/**
 * Stream the entire source line (primarily for debugging).
 */
std::ostream &operator<<(std::ostream &os, const MathDocumentLine &mdl)
{
  unsigned long l1, l2;
  std::ostringstream lineNum;

  l1 = mdl.getStartLineNumber();
  l2 = mdl.getEndLineNumber();
  if (l1 == l2)
    lineNum << l1;
  else
    lineNum << l1 << "-" << l2;

  return (os << "[" << mdl.getFilename() << ":" << lineNum.str() << "] "
	  << mdl.getContent());
}


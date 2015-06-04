/**
 * @file MathDocumentLine.h
 * Header file for the class that holds one line from a source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_LINE_H__
#define __MATH_DOCUMENT_LINE_H__

#include <string>
#include <queue>

/**
 * A single line in the source document.
 */
class MathDocumentLine
{
 protected:
  std::string m_filename;
  unsigned long m_line;
  std::string m_content;

 public:
  MathDocumentLine (const std::string &source_filename, 
		    const unsigned long source_linenumber,
		    const std::string &text);

  operator std::string() { return m_content; }
};

#endif /* __MATH_DOCUMENT_LINE_H__ */

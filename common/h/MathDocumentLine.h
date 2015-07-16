/**
 * @file MathDocumentLine.h
 * Header file for the class that holds one line from a source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_LINE_H__
#define __MATH_DOCUMENT_LINE_H__

#ifndef __COMMON_INCLUDES_H__
#include "common-includes.h"
#endif

/**
 * A single line in the source document.
 */
class MathDocumentLine
{
 protected:
  std::string m_filename;
  unsigned long m_line1;
  unsigned long m_line2;
  std::string m_content;

 public:
  MathDocumentLine (const std::string &source_filename, 
		    const unsigned long source_linenumber1,
		    const unsigned long source_linenumber2,
		    const std::string &text);

  std::string getFilename (void) const;
  unsigned long getStartLineNumber (void) const;
  unsigned long getEndLineNumber (void) const;
  std::string getContent (void) const;

  friend std::ostream &operator<<(std::ostream &os, const MathDocumentLine &mdl);
};

#endif /* __MATH_DOCUMENT_LINE_H__ */

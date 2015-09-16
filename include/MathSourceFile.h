/**
 * @file MathSourceFile.h
 * Header file for the class that holds source documents
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_SOURCE_FILE_H__
#define __MATH_SOURCE_FILE_H__

#include <vector>
#include "MathDocumentLine.h"

/**
 * A MathSourceFile holds the contents of the source document(s).
 */
class MathSourceFile
{
 public:
  /**
   * Initiate processing of a math document from the given file.
   */
  void loadFromFile (const std::string &filename);

  /**
   * Load from a buffer, optionally providing a 'fake' filename
   * (defaults to "(memory)")
   */
  void loadFromBuffer (const std::string &buffer, const std::string &filename = "(memory)");

  const std::vector<MathDocumentLine> &getDocument (void) const;

 protected:
  std::vector<MathDocumentLine> m_document;

  /**
   * Pull document text into the m_document buffer.
   */
  void ingest (const std::string &filename, const std::string &buffer);

};

#endif /* __MATH_SOURCE_FILE_H__ */

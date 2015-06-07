/**
 * @file MathDocument.cpp
 * Implementation of the container holding the raw source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <string>

#include "MathDocument.h"
#include "logging.h"


/**
 * Loads a source document from the specified file.
 *
 * Does not perform any particular processing, except for assigning line 
 * numbers to the contents of the source document.
 *
 * @param filename Path and filename to input file to be processed.
 * @throw MathDocumentParseException when parse or interpretation errors arise
 * @throw MathDocumentFileException on I/O errors
 */
void MathDocument::loadFromFile (const std::string &filename)
{
  assert (!filename.empty());
  
  std::ifstream ifs;
  ifs.open(filename.c_str(), std::ios::in);
  if (!ifs) {
    BOOST_THROW_EXCEPTION (MathDocumentFileException() <<
			   mdx_filename_info(filename) <<
			   mdx_error_info("File open error") <<
			   mdx_liberrno_info(errno) <<
			   mdx_liberrmsg_info(strerror(errno)) <<
			   mdx_liberrfunction_info("ifstream::open"));
  }

  std::ostringstream buf; 
  buf << ifs.rdbuf(); 

  // In case there was some sort of read error in the process, fail
  if (!ifs.good()) {
    BOOST_THROW_EXCEPTION (MathDocumentFileException() <<
			   mdx_filename_info(filename) <<
			   mdx_error_info("File read error") <<
			   mdx_liberrno_info(errno) <<
			   mdx_liberrmsg_info(strerror(errno)) <<
			   mdx_liberrfunction_info("ifstream::rdbuf"));
  }

  ingest (filename, buf.str());
  ifs.close();
}

/**
 * Appends text from a buffer (sourced from 'filename') to the document.
 *
 * @param filename Name of current file
 * @param buffer Characters to be added to the current document
 * @throw MathDocumentParseException when parse or interpretation errors arise
 * @throw MathDocumentFileException on I/O errors
 */
void MathDocument::ingest (const std::string &filename, 
			   const std::string &buffer)
{
  static int nestLevel = 0;
  unsigned long lineNumber = 1L;
  unsigned long columnNumber = 0;
  std::string curLine;

  nestLevel++;

  std::string LI;
  for (int i = 0; i < nestLevel; i++)
    LI += " ";
  LOG_INFO << "Ingesting from " << filename;

  for (std::string::const_iterator it = buffer.begin(); 
       it != buffer.end(); ++it) {
    columnNumber++;

    char c = *it;

    if (c == '\n') {
      LOG_TRACE << LI << "(" << lineNumber << ") " << curLine;
      m_document.push_back (MathDocumentLine (filename, lineNumber, curLine));
      curLine.clear();
      columnNumber = 0;
      lineNumber++;
      continue;
    }



    // DEFAULT: Output source character as-is
    curLine += c;
  }

  LOG_DEBUG << LI << "Ingestion complete";
}

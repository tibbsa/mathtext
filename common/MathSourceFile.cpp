/**
 * @file MathSourceFile.cpp
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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "logging.h"
#include "MathExceptions.h"
#include "MathSourceFile.h"

namespace ba = boost::algorithm;


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
void MathSourceFile::loadFromFile (const std::string &filename)
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

  ifs.close();

  std::string file_contents (buf.str());

  // On DOS, line endings may be \r\n rather than just \n.  Get rid of the
  // \r characters as we do not care about them.
  boost::replace_all(file_contents, "\r", "");
  ingest (filename, file_contents);
}

/**
* Appends text from a buffer (sourced from 'filename') to the document.
*
* @param filename Name of current file
* @param buffer Characters to be added to the current document
* @throw MathSourceFileParseException when parse or interpretation errors arise
* @throw MathSourceFileFileException on I/O errors
* Generates a string of spaces to help with log indentation.
*/
void MathSourceFile::ingest (const std::string &filename,
			     const std::string &buffer)
{
  unsigned long lineNumber = 1;
  unsigned long continuedLineStartedNumber = 0;
  std::string curLine;

  for (unsigned i = 0; i < buffer.length(); i++, lineNumber++) {
    // Grab a whole line of text
    int eolPos = buffer.find("\n", i);
    std::string temp = buffer.substr(i, eolPos - i);

    // Only trim from the right side, where trailing spaces should be
    // meaningless. Spaces on the left side might well be important 
    // in certain contexts (e.g. $PrintVerbatim sections)
    ba::trim_right(temp);

    // Advance to the end of the current line
    i = eolPos;

    // Handle "continuation lines".  If this line ended in a \ then 
    // combine it with the next line.
    if (temp.length() > 1 && temp.at (temp.length() - 1) == '\\') {
      if (!continuedLineStartedNumber)
	continuedLineStartedNumber = lineNumber;
      
      // Remove the continuation character from the line
      temp.erase(temp.end() - 1, temp.end());

      curLine = curLine + temp;
      continue;
    }


    // Handle "include" statements. These appear at the left margin and 
    // follow the format:
    //
    // #include <filename>\n
    if (strBeginsWith(temp, "#include ")) {
      static int numIncludeLevels = 0;

      if (++numIncludeLevels >= 5) {
	BOOST_THROW_EXCEPTION (
	  MathDocumentParseException() <<
	  mdx_filename_info(filename) <<
	  mdx_lineno_info(lineNumber) <<
	  mdx_error_info("You cannot nest #included files more than five levels deep!"));
      }

      // Advance beyond the "#include " command
      std::string include_filename = temp.substr(9);
      ba::trim(include_filename);

      // Try to load the include filename
      try { 
	loadFromFile(include_filename);
      } 
      catch (MathDocumentException &e) {
	// If an error of any sort occurred while loading the 'include' 
	// file, add info on how that file got included in the first place
	// to point the user to the source of the problem.
	std::string const *file = boost::get_error_info<mdx_filename_info>(e);

	// If an exception occured while loading or parsing, we should 
	// already have a filename reported. (Sanity check.)
	assert (file != NULL);

	std::string file_revised = *file;
	file_revised.append (boost::str(
			     boost::format(" (included by \"%s\" at line %u)")
			     % filename % lineNumber));

	e << mdx_filename_info(file_revised);
	throw;
      }

      continue;
    }

    curLine = curLine + temp;

    if (!continuedLineStartedNumber)
      continuedLineStartedNumber = lineNumber;

    m_document.push_back (MathDocumentLine (filename, 
					    continuedLineStartedNumber,
					    lineNumber, 
					    curLine));
    curLine.clear();
    continuedLineStartedNumber = 0;
  }
}

/* ========================= PRIVATE FUNCTIONS =========================== */

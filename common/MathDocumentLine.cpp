/**
 * @file MathDocument.cpp
 * Implementation of the container holding one line of the raw source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <string>

#include "MathDocumentLine.h"

/**
 * Sets up a new math document line object.
 *
 * @param source_filename Filename of document line loaded from
 * @param source_linenumber Number of line in file
 * @param text Contents of the line
 */
MathDocumentLine::MathDocumentLine (const std::string &source_filename,const unsigned long source_linenumber, const std::string &text) : m_filename(source_filename), m_line(source_linenumber), m_content(text)
{
}

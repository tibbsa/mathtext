/**
 * @file MathDocument.h
 * Header file for the class that holds source documents
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_H__
#define __MATH_DOCUMENT_H__

#include <string>
#include <queue>

#include <boost/exception/all.hpp>

#include "MathDocumentLine.h"


/**
 * A MathDocument holds the contents of the source document(s).
 */
class MathDocument
{
 protected:
  std::queue<MathDocumentLine> m_document;

 public:
  void loadFromFile (const std::string &filename);
};

/**
 * Exception handling
 */
typedef boost::error_info<struct mdx_filename,std::string> mdx_filename_info;
typedef boost::error_info<struct mdx_lineno,unsigned long> mdx_lineno_info;
typedef boost::error_info<struct mdx_error,std::string> mdx_error_info;
typedef boost::error_info<struct mdx_liberrno,int> mdx_liberrno_info;
typedef boost::error_info<struct mdx_liberrmsg,std::string> mdx_liberrmsg_info;
typedef boost::error_info<struct mdx_libfunction,std::string> mdx_liberrfunction_info;

class MathDocumentException : public boost::exception, public std::exception {};
class MathDocumentFileException : public MathDocumentException {};
class MathDocumentParseException : public MathDocumentException {};

#endif /* __MATH_DOCUMENT_H__ */

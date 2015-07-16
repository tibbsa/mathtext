/**
 * @file MathExceptions.h
 * Header file for exception handling out of the math converter code.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_EXCEPTIONS_H__
#define __MATH_EXCEPTIONS_H__

#include <boost/exception/all.hpp>

class MathDocumentElement;

/**
 * Exception handling
 */
typedef boost::error_info<struct mdx_filename,std::string> mdx_filename_info;
typedef boost::error_info<struct mdx_lineno,unsigned long> mdx_lineno_info;
typedef boost::error_info<struct mdx_error,std::string> mdx_error_info;
typedef boost::error_info<struct mdx_liberrno,int> mdx_liberrno_info;
typedef boost::error_info<struct mdx_liberrmsg,std::string> mdx_liberrmsg_info;
typedef boost::error_info<struct mdx_libfunction,std::string> mdx_liberrfunction_info;
typedef boost::error_info<struct mdx_element,MathDocumentElement> mdx_element_info;

class MathDocumentException : public boost::exception, public std::exception {};
class MathDocumentFileException : public MathDocumentException {};
class MathInterpreterException : public MathDocumentException {};
class MathRenderException : public MathDocumentException {};

#endif /* __MATH_EXCEPTIONS_H__ */

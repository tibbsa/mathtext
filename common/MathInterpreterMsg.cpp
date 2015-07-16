/**
 * @file MathInterpreterMsg.cpp
 * Implementation of the container holding a message generated by the interpreter
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathInterpreterMsg.h"

/**
 * Sets up a new math document message object.
 *
 * @param category Message category
 * @param msgCode Application-defined numeric message code
 * @param source_filename Filename of document line loaded from
 * @param source_linenumber1 Number of line in file
 * @param source_linenumber2 Number of line (if continuation line) in file
 * @param text Contents of the line
 */
MathInterpreterMsg::MathInterpreterMsg (const MathInterpreterMsg::Category category,
					const MathInterpreterMsg::Code msgCode,
					const std::string &source_filename,
					const unsigned long source_linenumber1, 
					const unsigned long source_linenumber2, 
					const std::string &msg) : m_category(category), m_code(msgCode), m_filename(source_filename), m_line1(source_linenumber1), m_line2(source_linenumber2), m_message(msg)
{
}

/**
 * Retrieve the category associated with this message.
 *
 * @return Category associated with this message
 */
MathInterpreterMsg::Category MathInterpreterMsg::getCategory (void) const
{
  return m_category;
}

/**
 * Retrieve the numberic message code associated with this message.
 */
MathInterpreterMsg::Code MathInterpreterMsg::getCode (void) const
{
  return m_code;
}

/**
 * Retrieve the filename associated with this message.
 *
 * @return Filename associated with this message
 */
std::string MathInterpreterMsg::getFilename (void) const
{
  return m_filename;
}

/**
 * Retrieve the starting line number associated with this message.
 *
 * @return First source line number associated with this message.
 */
unsigned long MathInterpreterMsg::getStartLineNumber (void) const
{
  return m_line1;
}

/**
 * Retrieve the ending line number associated with this message.
 *
 * @return Final source line numer associated with this message.
 */
unsigned long MathInterpreterMsg::getEndLineNumber (void) const
{
  return m_line2;
}

/**
 * Retrieve the contents of the line.
 *
 * @return Contents of the line.
 */
std::string MathInterpreterMsg::getMessage (void) const
{
  static std::map<MathInterpreterMsg::Code,std::string> error_map = boost::assign::map_list_of 
    (NESTED_TEXT_MODE, "Text mode indicator (&&) found while already in text mode")
    (NESTED_MATH_MODE, "Math mode indicator ($$) found while already in math mode")
    (SUSPECT_MATH_IN_TEXT, "Suspected math symbols in a text passage")
    (SUSPECT_TEXT_IN_MATH, "Suspected text material in a math passage")
    (SUSPECT_FRACTION, "Suspect missing open fraction symbol (@)")
    (UNKNOWN_GREEK, "Unknown Greek character symbol")
    (FRACTION_NOT_TERMINATED, "Fraction terminator symbol (#) appears to be missing")
    (EXPONENT_NOT_TERMINATED, "Exponent begins with opening paren '(' but is never terminated with a closing paren ')'")
    (SUBSCRIPT_NOT_TERMINATED, "Subscript begins with opening paren '(' but is never terminated with a closing paren ')'")
    (ROOT_INDEX_NOT_TERMINATED, "Root includes a complex index with opening bracket '[' but is never terminated with a closing bracket ']'")
    (ROOT_NOT_TERMINATED, "Root begins with opening paren '(' but is never terminated with a closing paren ')'")
    (MODIFIER_NOT_TERMINATED, "Text attached to a symbol begins with opening paren '(' but is never terminated with a closing paren ')'")
    ;

  assert (error_map.count(m_code) == 1);

  return (boost::str(boost::format("%s - %s") % error_map[m_code] % m_message));
}

/**
 * Stream the entire source line (primarily for debugging).
 */
std::ostream &operator<<(std::ostream &os, const MathInterpreterMsg &mdm)
{
  unsigned long l1, l2;
  std::ostringstream lineNum;
  std::string categoryStr;

  switch (mdm.getCategory()) {
  case MathInterpreterMsg::INFO: 
    categoryStr = "FYI"; 
    break;
    
  case MathInterpreterMsg::NOTICE:
    categoryStr = "NOTE"; 
    break;

  case MathInterpreterMsg::WARNING:
    categoryStr = "WARNING";
    break;

  case MathInterpreterMsg::ERROR:
    categoryStr = "ERROR";
    break;
  }

  l1 = mdm.getStartLineNumber();
  l2 = mdm.getEndLineNumber();
  if (l1 == l2)
    lineNum << l1;
  else
    lineNum << l1 << "-" << l2;

  return (os << categoryStr << ": " << mdm.getMessage() 
	     << " (at line " << lineNum.str() 
	  << " in " << mdm.getFilename() << ")");
}


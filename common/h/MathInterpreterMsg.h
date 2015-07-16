/**
 * @file MathInterpreterMsg.h
 * Header file for the class that holds interpretation messages
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_INTERPRETER_MSG_H__
#define __MATH_INTERPRETER_MSG_H__

#ifndef __COMMON_INCLUDES_H__
#include "common-includes.h"
#endif

/**
 * A single message generated by the math interpreter
 */
class MathInterpreterMsg
{
 public:
  typedef enum { INFO, NOTICE, WARNING, ERROR } Category;
  typedef enum { NESTED_TEXT_MODE, NESTED_MATH_MODE, 
		 SUSPECT_MATH_IN_TEXT, SUSPECT_TEXT_IN_MATH,
		 SUSPECT_FRACTION, 
		 UNKNOWN_GREEK,
		 FRACTION_NOT_TERMINATED,
		 EXPONENT_NOT_TERMINATED,
		 SUBSCRIPT_NOT_TERMINATED,
		 ROOT_INDEX_NOT_TERMINATED,
		 ROOT_NOT_TERMINATED,
		 MODIFIER_MISSING_ARGUMENT,
		 MODIFIER_NOT_TERMINATED } Code;
		 
 protected:
  Category m_category;
  Code m_code;
  std::string m_filename;
  unsigned long m_line1;
  unsigned long m_line2;
  std::string m_message;

 public:
  MathInterpreterMsg (const MathInterpreterMsg::Category category,
		      const MathInterpreterMsg::Code msgCode, 
		      const std::string &source_filename, 
		      const unsigned long source_linenumber1,
		      const unsigned long source_linenumber2,
		      const std::string &msg);

  MathInterpreterMsg::Category getCategory (void) const;
  MathInterpreterMsg::Code getCode (void) const;
  std::string getFilename (void) const;
  unsigned long getStartLineNumber (void) const;
  unsigned long getEndLineNumber (void) const;
  std::string getMessage (void) const;

  friend std::ostream &operator<<(std::ostream &os, const MathInterpreterMsg &mdm);
};

#endif /* __MATH_INTERPRETER_MSG_H__ */

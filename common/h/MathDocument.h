/**
 * @file MathDocument.h
 * Header file for the class that holds interpreted math documents
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_H__
#define __MATH_DOCUMENT_H__

#include <vector>

#include <boost/shared_ptr.hpp>

#include "MathDocumentElements.h"
#include "MathDocumentMsg.h"
#include "MathSourceFile.h"

/**
 * Macros to help with message adding
 */
#define MSG_INFO(code,msg) addMessage(MathDocumentMsg::INFO, code, msg)
#define MSG_INFOX(code) addMessage(MathDocumentMsg::INFO, code)
#define MSG_NOTICE(code,msg) addMessage(MathDocumentMsg::NOTICE, code, msg)
#define MSG_NOTICEX(code) addMessage(MathDocumentMsg::NOTICE, code)
#define MSG_WARNING(code,msg) addMessage(MathDocumentMsg::WARNING, code, msg)
#define MSG_WARNINGX(code) addMessage(MathDocumentMsg::WARNING, code)
#define MSG_ERROR(code,msg) addMessage(MathDocumentMsg::WARNING, code, msg)
#define MSG_ERRORX(code) addMessage(MathDocumentMsg::WARNING, code)

/**
 * Message codes: 1xxx are notices, 2xxx are warnings, 3xxx are errors
 */
#define MDM_NESTED_TEXT_MODE 2000
#define MDM_NESTED_MATH_MODE 2001
#define MDM_SUSPECT_MATH_IN_TEXT 2010
#define MDM_FRACTION_NOT_TERMINATED 3000
#define MDM_EXPONENT_NOT_TERMINATED 3001

/**
 * A MathDocument holds the contents of an interpreted math file./
 */

class MathDocument
{
 protected:
  MDEVector m_document;
  std::vector<MathDocumentMsg> m_messages;

  /* Translation "current status" parameters */
  const MathDocumentLine *pCurLine;
  bool inTextBlock; // lines default to being 'text' lines if enabled
  bool inTextMode; // whether translator is currently seeing 'text'
  bool isStartOfLine;
  unsigned long textBlockBeganLine;
  unsigned long mathBlockBeganLine;

  /* Interpretation functions */
  void interpretLine (const MathDocumentLine &mdl);
  MDEVector interpretBuffer (const std::string &buffer);
  bool interpretItemNumber (MDEVector &target, const std::string &src, size_t &i);
  bool interpretOperator (MDEVector &target, const std::string &src, size_t &i);
  bool interpretComparator (MDEVector &target, const std::string &src, size_t &i);
  bool interpretFraction (MDEVector &target, const std::string &src, size_t &i);
  bool interpretExponent (MDEVector &target, const std::string &src, size_t &i);

  MathDocumentElementPtr makeGeneric (const std::string &buffer);

  /* Support functions */
  void sniffTextForMath (const std::string &buffer);

  /* Reporting functions */
  void addMessage (const MathDocumentMsg::MDMCategory category, 
		   const unsigned long msgCode,
		   const std::string &msg = std::string());

 public:
  MathDocument();
  void interpret (const MathSourceFile &src);
  bool haveMessages (void) const;
  const std::vector<MathDocumentMsg> &getMessages (void) const;

  static std::string getErrorMessage (const unsigned long errorCode);

  friend class MathRenderer;
};

#endif /* __MATH_DOCUMENT_H__ */

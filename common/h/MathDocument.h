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
 * Message codes
 */
#define MDM_NESTED_TEXT_MODE 1000
#define MDM_NESTED_MATH_MODE 1001

/**
 * A MathDocument holds the contents of an interpreted math file./
 */
typedef boost::shared_ptr<MathDocumentElement> MathDocumentElementPtr;
typedef std::vector<MathDocumentElementPtr> MDEVector;
typedef boost::shared_ptr<MDEVector> MDEVectorPtr;

class MathDocument
{
 protected:
  MDEVector m_document;
  std::vector<MathDocumentMsg> m_messages;

  const MathDocumentLine *pCurLine;
  bool inTextBlock; // lines default to being 'text' lines if enabled
  bool inTextMode; // whether translator is currently seeing 'text'
  unsigned long textBlockBeganLine;
  unsigned long mathBlockBeganLine;

  void interpretLine (const MathDocumentLine &mdl);
  MDEVector interpretBuffer (const std::string &buffer);
  bool interpretOperatorSign (MDEVector &target, const std::string &src, int &i);

  MathDocumentElementPtr makeGeneric (const std::string &buffer);

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

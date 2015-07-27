/**
 * @file MathInterpreter.h
 * Header file for the class that interprets source documents into the 
 * internally represented math document.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_INTERPRETER_H__
#define __MATH_INTERPRETER_H__

#ifndef __COMMON_INCLUDES_H__
#include "common-includes.h"
#endif

#include "MathDocument.h"
#include "MathDocumentElements.h"
#include "MathInterpreterMsg.h"
#include "MathSourceFile.h"

/**
 * Macros to help with message adding
 */
#define MSG_INFO(code,msg) addMessage(MathInterpreterMsg::MI_INFO, MathInterpreterMsg::code, msg)
#define MSG_INFOX(code) addMessage(MathInterpreterMsg::MI_INFO, MathInterpreterMsg::code)
#define MSG_NOTICE(code,msg) addMessage(MathInterpreterMsg::MI_NOTICE, MathInterpreterMsg::code, msg)
#define MSG_NOTICEX(code) addMessage(MathInterpreterMsg::MI_NOTICE, MathInterpreterMsg::code)
#define MSG_WARNING(code,msg) addMessage(MathInterpreterMsg::MI_WARNING, MathInterpreterMsg::code, msg)
#define MSG_WARNINGX(code) addMessage(MathInterpreterMsg::MI_WARNING, MathInterpreterMsg::code)
#define MSG_ERROR(code,msg) addMessage(MathInterpreterMsg::MI_ERROR, MathInterpreterMsg::code, msg)
#define MSG_ERRORX(code) addMessage(MathInterpreterMsg::MI_ERROR, MathInterpreterMsg::code)

class MathInterpreter
{
 protected:
  const MathSourceFile &m_src;
  MathDocument &m_doc;
  std::vector<MathInterpreterMsg> m_messages;
  std::vector<std::string> m_knownCommands;

  /* Translation "current status" parameters */
  const MathDocumentLine *m_pCurLine;
  bool m_inTextBlock; // lines default to being 'text' lines if enabled
  bool m_inTextMode; // whether translator is currently seeing 'text'
  bool m_isStartOfLine;
  unsigned long m_blockBeganLineNum; // line at which current block began

  /* Interpretation functions */
  void interpretLine (const MathDocumentLine &mdl);
  MDEVector interpretBuffer (const std::string &buffer);
  bool interpretCommand (MDEVector &target, const std::string &src, size_t &i);
  bool interpretNumber (MDEVector &target, const std::string &src, size_t &i);
  bool interpretItemNumber (MDEVector &target, const std::string &src, size_t &i);
  bool interpretOperator (MDEVector &target, const std::string &src, size_t &i);
  bool interpretComparator (MDEVector &target, const std::string &src, size_t &i);
  bool interpretGreekLetter (MDEVector &target, const std::string &src, size_t &i);
  bool interpretSymbol (MDEVector &target, const std::string &src, size_t &i);
  bool interpretModifier (MDEVector &target, const std::string &src, size_t &i);
  bool interpretFraction (MDEVector &target, const std::string &src, size_t &i);
  bool interpretExponent (MDEVector &target, const std::string &src, size_t &i);
  bool interpretSubscript (MDEVector &target, const std::string &src, size_t &i);
  bool interpretRoot (MDEVector &target, const std::string &src, size_t &i);

  MathDocumentElementPtr makeGeneric (const std::string &buffer);

  /* Support functions */
  void sniffTextForMath (const std::string &buffer);
  bool extractItem (std::string &target, const std::string &src, size_t &i, 
		    const std::string &terminators = ",+/*=<>()[]{} ~@#");
  bool extractGroup (std::string &target, const std::string &src, size_t &i,
		     const std::string &groupOpen = "(", 
		     const std::string &groupClose = ")",
		     const bool retainGroupDelims = false);

  /* Reporting functions */
  void addMessage (const MathInterpreterMsg::Category category, 
		   const MathInterpreterMsg::Code msgCode,
		   const std::string &msg = std::string());

 public:
  MathInterpreter(const MathSourceFile &srcFile, MathDocument &targetDoc);
  void interpret (void);
  void registerCommand (const std::string &cmd);
  bool isCommand (const std::string &cmd) const;
  bool haveMessages (void) const;
  const std::vector<MathInterpreterMsg> &getMessages (void) const;
};

#endif /* __MATH_INTERPRETER_H__ */

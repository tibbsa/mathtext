/**
 * @file MathDocument.cpp
 * Implementation of the container holding the interpreted source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <map>

#include <boost/assign.hpp>
#include <boost/format.hpp>

#include "logging.h"
#include "MathDocument.h"

/* ========================= PUBLIC FUNCTION =============================== */

/**
 * Sets up a new, blank math document
 */
MathDocument::MathDocument()
{
}

/**
 * Interprets a source document
 *
 * @param src A ready-to-go math source file for interpretation
 * @throw MathDocumentParseException when parse or interpretation errors arise
 */
void MathDocument::interpret (const MathSourceFile &src)
{
  LOG_TRACE << "enter MathDocument::interpret";
  logIncreaseIndent();

  // Reset interpreter setup
  m_document.clear();
  m_messages.clear();
  inTextMode = false;
  textBlockBeganLine = 0;
  mathBlockBeganLine = 0;

  for (std::vector<MathDocumentLine>::const_iterator it = src.m_document.begin();
       it != src.m_document.end(); ++it) {

    interpretLine(*it);
  }
  
  logDecreaseIndent();
  LOG_TRACE << "exit MathDocument::interpret";

  LOG_TRACE << "=============================================================";

  for (MDEVector::const_iterator it = m_document.begin(); 
       it != m_document.end(); ++it) {
    MathDocumentElementPtr e = *it;
    LOG_TRACE << *e;
  }

}

/**
 * Returns true if there are one or more 'interpretaiton messages' to report
 */
bool MathDocument::haveMessages (void) const
{
  return (!m_messages.empty());
}

/**
 * Returns a reference to the messages array
 */
const std::vector<MathDocumentMsg> &MathDocument::getMessages (void) const
{
  return m_messages;
}

/**
 * Returns a textual description of a particular error message.
 */
std::string MathDocument::getErrorMessage (const unsigned long errorCode)
{
  static std::map<unsigned long,std::string> error_map = boost::assign::map_list_of 
    (MDM_NESTED_TEXT_MODE, "Text mode indicator (&&) found while already in text mode")
    (MDM_NESTED_MATH_MODE, "Math mode indicator ($$) found while already in math mode")
    ;

  assert (error_map.count(errorCode) > 0);
  return error_map [errorCode];
}

/**
 * Interprets a single line in the document
 *
 * @param mdl A single line in the document
 */
void MathDocument::interpretLine (const MathDocumentLine &mdl)
{
  MDEVector elements;

  LOG_TRACE << "enter MathDocument::interpretLine(" << mdl << ")";
  logIncreaseIndent();

  pCurLine = &mdl;
  std::string buffer = mdl.getContent();

  /* Set defaults for this line */
  inTextMode = inTextBlock;

  /*
   * MAIN MODE SWITCHES
   */
  if (buffer == "&&") {
    if (!inTextBlock) {
      LOG_TRACE << "* entering text block mode";
      inTextBlock = true;
      inTextMode = true;
      textBlockBeganLine = mdl.getStartLineNumber();
      mathBlockBeganLine = 0;

      m_document.push_back (boost::make_shared<MDE_TextModeMarker>());
    } else {
      MSG_WARNING(MDM_NESTED_TEXT_MODE, boost::str(boost::format("text block began at line %u") % textBlockBeganLine));
    }

    goto EOL;
  }

  if (buffer == "$$") {
    if (inTextBlock) {
      LOG_TRACE << "* entering math block mode";
      inTextBlock = false;
      inTextMode = false;
      mathBlockBeganLine = mdl.getStartLineNumber();
      textBlockBeganLine = 0;

      m_document.push_back (boost::make_shared<MDE_MathModeMarker>());
    } else {
      MSG_WARNING(MDM_NESTED_MATH_MODE, boost::str(boost::format("math block began at line %u") % mathBlockBeganLine));
    }

    goto EOL;
  }

  elements = interpretBuffer (buffer);
  elements.push_back (boost::make_shared<MDE_LineBreak>());

  LOG_TRACE << "---- Decoded line ---------";
  for (MDEVector::const_iterator it = elements.begin(); 
       it != elements.end(); ++it) {
    MathDocumentElementPtr e = *it;
    LOG_TRACE << *e;
  }
  LOG_TRACE << "--------------------------";

  m_document.insert(m_document.end(), elements.begin(), elements.end());

 EOL:
  logDecreaseIndent();
  LOG_TRACE << "exit MathDocument::interpretLine";
}

/**
 * Interprets the selected text and returns an array of 
 * document elements.
 */
#define PUSH_CATCH_BUFFER { if (!catch_buffer.empty()) {  elements.push_back (makeGeneric (catch_buffer));  catch_buffer.erase(); } }

MDEVector MathDocument::interpretBuffer (const std::string &buffer)
{
  std::string catch_buffer;
  MDEVector elements;

  LOG_TRACE << "enter interpretBuffer(" << buffer << ")";
  logIncreaseIndent();

  for (int i = 0; i < buffer.length(); i++) {
    char c = buffer[i];
    LOG_TRACE << "At pos " << i << ", char '" << c << "', " << (inTextBlock ? "T" : "M") << (inTextMode ? "t" : "m");
    logIncreaseIndent();
 
    /**
     * Mode changes: text to math or math to text. Dump what we have up to 
     * this point whenever there is a mode change.
     */
    if (c == '$') {
      if (inTextMode) {
	PUSH_CATCH_BUFFER;
	inTextMode = false;
	elements.push_back (boost::make_shared<MDE_MathModeMarker>());
      } else {
	MSG_WARNINGX(MDM_NESTED_MATH_MODE);
      }

      goto NextChar;
    }

    if (c == '&') {
      if (!inTextMode) {
	PUSH_CATCH_BUFFER;
	inTextMode = true;
	elements.push_back (boost::make_shared<MDE_TextModeMarker>());
      } else {
	MSG_WARNINGX(MDM_NESTED_TEXT_MODE);
      }

      goto NextChar;
    }

    /**
     * Default action: Save the unknown character to be added later as a 
     * generic text/math block.
     */
    catch_buffer.push_back (c);

  NextChar:
    logDecreaseIndent();
    continue;
  }

  PUSH_CATCH_BUFFER;
 
  logDecreaseIndent();
  LOG_TRACE << "exit interpretBuffer";
  return elements;
}

/**
 * Creates a 'generic text/math' block from the characters in buffer
 */
MathDocumentElementPtr MathDocument::makeGeneric (const std::string &buffer)
{
  MathDocumentElementPtr e;

  LOG_TRACE << "entering makeGeneric(" << buffer << "), TextMode=" << inTextMode;
  logIncreaseIndent();

  if (inTextMode)
    e = boost::make_shared<MDE_TextBlock>(buffer);
  else
    e = boost::make_shared<MDE_MathBlock>(buffer);

  logDecreaseIndent();
  LOG_TRACE << "exiting makeGeneric";

  return e;
}

/**
 * Adds a warning/info/notice message to the log for later perusal.
 */
void MathDocument::addMessage (const MathDocumentMsg::MDMCategory category,
			       const unsigned long msgCode,
			       const std::string &msg)
{
  assert (pCurLine != NULL);

  std::string message;
  message = getErrorMessage(msgCode);
  if (!msg.empty())
    message = message + " - " + msg;

  MathDocumentMsg mdm(category, 
		      msgCode,
		      pCurLine->getFilename(),
		      pCurLine->getStartLineNumber(),
		      pCurLine->getEndLineNumber(),
		      message);

  m_messages.push_back(mdm);
  LOG_INFO << "MSG: " << mdm;
}

/* ========================= PRIVATE FUNCTIONS =========================== */

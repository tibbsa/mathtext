/**
 * @file MathInterpreter.cpp
 * Implementation of the math interpreter
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include "logging.h"

#include "MathExceptions.h"
#include "MathInterpreter.h"

namespace ba = boost::algorithm;

/* ========================= PUBLIC FUNCTION =============================== */

/**
 * Sets up a new, blank math document
 *
 * @param src A ready-to-go math source file for interpretation
 * @param doc A MathDocument where the interpretation should be stored
 */
MathInterpreter::MathInterpreter (const MathSourceFile &srcFile,
				  MathDocument &targetDoc)
  : m_src(srcFile), m_doc(targetDoc)
{
}


/**
 * Interprets a source document
 *
 * @throw MathInterpreterException when parse or interpretation errors arise
 */
void MathInterpreter::interpret (void)
{
  LOG_TRACE << "enter MathInterpreter::interpret";
  logIncreaseIndent();

  // Reset interpreter setup
  m_inTextMode = false;
  m_inTextBlock = false;
  m_blockBeganLineNum = 1;

  const std::vector<MathDocumentLine> &srcDoc = m_src.getDocument();
  for (std::vector<MathDocumentLine>::const_iterator it = srcDoc.begin();
       it != srcDoc.end(); ++it) {

    m_doc.addElementPtr (boost::make_shared<MDE_SourceLine>(it->getFilename(),
							    it->getStartLineNumber(),
							    it->getEndLineNumber(),
							    it->getContent()));
    interpretLine(*it);
  }

  logDecreaseIndent();
  LOG_TRACE << "exit MathInterpreter::interpret";

  LOG_TRACE << "=============================================================";

  for (MDEVector::const_iterator it = m_doc.getDocument().begin();
       it != m_doc.getDocument().end(); ++it) {
    MathDocumentElementPtr e = *it;
    LOG_TRACE << *e;
  }
}

/**
 * Called, typically by a renderer, to add a command to the 'known' commands
 * list.
 */
void MathInterpreter::registerCommand (const std::string &cmd)
{
  assert (!isCommand(cmd));
  m_knownCommands.push_back(cmd);
  LOG_TRACE << "MathInterpreter::registerCommand(" << cmd << ")";
}

void MathInterpreter::registerCommands (const std::vector<std::string> &commands)
{
  BOOST_FOREACH (const std::string &cmd, commands) {
    registerCommand(cmd);
  }
}


/**
 * Returns TRUE if the specified 'command' is known to the interpreter.
 */
bool MathInterpreter::isCommand (const std::string &cmd) const
{
  return (std::count(m_knownCommands.begin(), m_knownCommands.end(), cmd) == 1);
}

/**
 * Returns true if there are one or more 'interpretaiton messages' to report
 */
bool MathInterpreter::haveMessages (void) const
{
  return (!m_messages.empty());
}

/**
 * Returns a reference to the messages array
 */
const std::vector<MathInterpreterMsg> &MathInterpreter::getMessages (void) const
{
  return m_messages;
}

/**
 * Interprets a single line in the document
 *
 * @param mdl A single line in the document
 */
void MathInterpreter::interpretLine (const MathDocumentLine &mdl)
{
  MDEVector elements;

  LOG_TRACE << "enter MathInterpreter::interpretLine(" << mdl << ")";
  logIncreaseIndent();

  m_pCurLine = &mdl;
  std::string buffer = mdl.getContent();

  /* Set defaults for this line */
  m_inTextMode = m_inTextBlock;
  m_isStartOfLine = true;

  /*
   * MAIN MODE SWITCHES
   */
  if (buffer == "&&") {
    if (!m_inTextBlock) {
      LOG_TRACE << "* entering text block mode";
      m_inTextBlock = true;
      m_inTextMode = true;
      m_blockBeganLineNum  = mdl.getStartLineNumber();

      m_doc.addElementPtr (boost::make_shared<MDE_TextModeMarker>(MDE_TextModeMarker::BLOCK_MARKER));
    } else {
      MSG_WARNING(NESTED_TEXT_MODE, boost::str(boost::format("text block began at line %u") % m_blockBeganLineNum));
    }

    goto EOL;
  }

  if (buffer == "$$") {
    if (m_inTextBlock) {
      LOG_TRACE << "* entering math block mode";
      m_inTextBlock = false;
      m_inTextMode = false;
      m_blockBeganLineNum = mdl.getStartLineNumber();
      m_doc.addElementPtr (boost::make_shared<MDE_MathModeMarker>(MDE_MathModeMarker::BLOCK_MARKER));
    } else {
      MSG_WARNING(NESTED_MATH_MODE, boost::str(boost::format("math block began at line %u") % m_blockBeganLineNum));
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

  m_doc.addElements(elements);

 EOL:
  logDecreaseIndent();
  LOG_TRACE << "exit MathInterpreter::interpretLine";
}

/**
 * Interprets the selected text and returns an array of
 * document elements.
 */
#define PUSH_CATCH_BUFFER { if (!boost::trim_copy(catch_buffer).empty()) { elements.push_back (makeGeneric(catch_buffer)); } catch_buffer.erase(); }

#define ATTEMPT(class) { if (interpret##class (temp_elements, buffer, i)) { PUSH_CATCH_BUFFER; elements.insert (elements.end(), temp_elements.begin(), temp_elements.end()); goto NextChar; } }

MDEVector MathInterpreter::interpretBuffer (const std::string &buffer)
{
  static unsigned long interpreter_recursion_level = 0;
  std::string catch_buffer;
  MDEVector elements;

  interpreter_recursion_level++;
  LOG_TRACE << "enter interpretBuffer lvl " << interpreter_recursion_level << " (" << buffer << ")";
  logIncreaseIndent();

  for (size_t i = 0; i < buffer.length(); /* i++ done below */) {
    MDEVector temp_elements;

    char c = buffer[i];
    LOG_TRACE << "At pos " << i << ", char '" << c << "', " << (m_inTextBlock ? "T" : "M") << (m_inTextMode ? "t" : "m");
    logIncreaseIndent();

    if (m_isStartOfLine) {
      ATTEMPT(Command);
    }

    /**
     * Mode changes: text to math or math to text. Dump what we have up to
     * this point whenever there is a mode change.
     */
    if (c == '$') {
      // Do not consider `$ to be a math mode change: this is a dollar sign
      if (!m_inTextMode && i > 0 && buffer [i - 1] == '`')
        goto DefaultAction;

      if (m_inTextMode) {
	      LOG_TRACE << "* entering math mode; pushing '" << catch_buffer << "'";
	      PUSH_CATCH_BUFFER;
	      m_inTextMode = false;
	      elements.push_back (boost::make_shared<MDE_MathModeMarker>(MDE_MathModeMarker::SEGMENT_MARKER));
      } else {
	      LOG_TRACE << "! attempt to enter math mode while in math mode";
	      MSG_WARNINGX(NESTED_MATH_MODE);
      }

      goto AdvanceNextChar;
    }

    if (c == '&') {
      if (!m_inTextMode) {
        LOG_TRACE << "* entering text mode; pushing '" << catch_buffer << "'";
        PUSH_CATCH_BUFFER;
        m_inTextMode = true;
        elements.push_back(boost::make_shared<MDE_TextModeMarker>(MDE_TextModeMarker::SEGMENT_MARKER));
      }
      else {
        LOG_TRACE << "! attempt to enter math mode while in text mode";
        MSG_WARNINGX(NESTED_TEXT_MODE);
      }

      goto AdvanceNextChar;
    }

    // If this is the first thing we're seeing on the line, check to see
    // if the first blob appears to be an 'item number' (as might appear
    // in homework).
    if (m_isStartOfLine && !m_inTextMode) {
      ATTEMPT(ItemNumber);
      
      // Don't consider the line to have started until we see a non-space character.
      if (!isspace(c))
        m_isStartOfLine = false;
    }

    if (m_inTextMode)
      goto HandleTextBlocks;

    ATTEMPT(Group);
    ATTEMPT(Operator);
    ATTEMPT(Number);
    ATTEMPT(Comparator);
    ATTEMPT(GreekLetter);
    ATTEMPT(Modifier); // Do this before symbols so `CJ gets matched
    ATTEMPT(Symbol);
    ATTEMPT(Fraction);
    ATTEMPT(Root); // do this before a subscript to avoid confusion
    ATTEMPT(Summation);
    ATTEMPT(Exponent);
    ATTEMPT(Subscript);

    goto DefaultAction;

  HandleTextBlocks:
    ;

  DefaultAction:
    ;

    /**
     * Default action: Save the unknown character to be added later as a
     * generic text/math block.
     */
    catch_buffer.push_back (c);

  AdvanceNextChar:
    i++;

  NextChar:
    logDecreaseIndent();
    continue;
  }

  PUSH_CATCH_BUFFER;

  interpreter_recursion_level--;

  logDecreaseIndent();
  LOG_TRACE << "exit interpretBuffer";
  return elements;
}

/**
 * Creates a 'generic text/math' block from the characters in buffer
 */
MathDocumentElementPtr MathInterpreter::makeGeneric (const std::string &buffer)
{
  MathDocumentElementPtr e;

  LOG_TRACE << "creating generic block(" << buffer << "), TextMode=" << m_inTextMode;

  if (m_inTextMode) {
    sniffTextForMath (buffer);
    e = boost::make_shared<MDE_TextBlock>(buffer);
  } else {
    e = boost::make_shared<MDE_MathBlock>(boost::trim_right_copy(buffer));
  }

  return e;
}

/**
 * Evaluates a block of text to determine whether it appears as though it
 * might contain mathematical material.  If it does, a warning is added to
 * the processing log.
 */
#define CONTAINS(c) (buffer.find(c) != std::string::npos)
void MathInterpreter::sniffTextForMath (const std::string &buffer)
{
  std::vector<std::string> suspicious_items;

  if (CONTAINS('@') && CONTAINS('~') && CONTAINS('#'))
    suspicious_items.push_back ("Fractions");

  if (CONTAINS('<') || CONTAINS('>') || CONTAINS('='))
    suspicious_items.push_back ("Signs of Comparison");

  if (CONTAINS("_/"))
    suspicious_items.push_back ("Roots");
  else if (CONTAINS("/_"))
    suspicious_items.push_back ("Angles");
  else if (CONTAINS('_'))
    suspicious_items.push_back ("Subsbcripts");

  if (CONTAINS('^'))
    suspicious_items.push_back ("Exponents");

  if (CONTAINS('|'))
    suspicious_items.push_back ("Absolute Values");

  if (!suspicious_items.empty()) {
    MSG_WARNING(SUSPECT_MATH_IN_TEXT, boost::str(boost::format("found %s in '%s'") % ba::join(suspicious_items, ", ") % buffer));
  }
}
#undef CONTAINS

/**
 * Extracts the next "item" from the input buffer, which could be:
 * - an entire fraction
 * - a number or other construct, up to the next terminator
 *
 * A semi-colon will termiante the extraction as well (and not be copied into
 * the target buffer).
 *
 * Returns TRUE on success, or FALSE on error.
 */
 bool MathInterpreter::extractItem (std::string &target,
				 const std::string &src,
				 size_t &i,
				 const std::string &terminators)
{
  std::string current_terminators = terminators;
  size_t pos = i;

  // Skip over any white space that might come before this group
  while (pos < src.length() && isspace(src[pos]))
    pos++;

  // Copy characters until we encounter any of the above-mentioned
  // terminators.
  //
  // The semi-colon is specifically designated as a terminator,
  // and should be skipped if it arises.
  //
  // Special cases:
  // - negative numbers (items can begin with a minus sign '-' but
  //   only in the first character)
  for (; pos < src.length(); pos++) {
    if (src [pos] == ';') {
      pos++;
      break;
    }

    // On other terminators, do not "lose them" -- they should wind
    // up in the final output.
    if (isOneOf (src [pos], current_terminators))
      break;

    if (pos == i)
      current_terminators += '-';

    target += src [pos];
  }

  i = pos;

  boost::trim(target);
  return (!target.empty());
}

/**
 * Extracts the next group of symbols, bounded by groupOpen/groupClose.
 *
 * Used to grab argments to exponents, etc. e.g. 2x^(1 + @1~y#)
 * If 'retainGroupDelims' is TRUE (default false), the group delimeters
 * 'groupOpen' and 'groupClose' are included in 'target'.
 *
 * Returns TRUE on success, FALSE on error (typically, an unclosed group).
 */
bool MathInterpreter::extractGroup (std::string &target,
				    const std::string &buffer,
				    size_t &i,
				    const std::string &groupOpen,
				    const std::string &groupClose,
				    bool retainGroupDelims)
{
  int groupNestingLevel = 0;
  bool foundTerminator = false;
  size_t pos = i;

  // Skip over any white space that might come before this group
  while (pos < buffer.length() && isspace(buffer[pos]))
    pos++;

  // Fail if the next character is not a group open indicator
  if (buffer.substr(pos, groupOpen.length()) != groupOpen)
    return false;

  pos += groupOpen.length();

  if (retainGroupDelims)
    target += groupOpen;

  for (; pos < buffer.length(); pos++) {
    // If we see a 'nested' group opening, we don't want to prematurely end
    // when that group ends.  Make note of it.
    if (buffer.substr(pos, groupOpen.length()) == groupOpen) {
      groupNestingLevel++;
    } else if (buffer.substr(pos, groupClose.length()) == groupClose) {
      if (!groupNestingLevel) {
	foundTerminator = true;
	pos += groupClose.length();
	break;
      }

      groupNestingLevel--;
    }

    // Add this character to the contents of the exponent
    target += buffer[pos];
  }

  i = pos;
  if (foundTerminator) {
    if (retainGroupDelims)
      target += groupClose;

    return true;
  }
  else
    return false;
}

/**
 * Extracts up to the next 'delim'.  But if we start with an open paren,
 * take that whole group regardless of whether it contains a comma.
 *
 * Returns TRUE on success, FALSE on error (typically, an unclosed group).
 */
bool MathInterpreter::extractToDelimiter (std::string &target,
					  const std::string &buffer,
					  size_t &i,
					  const std::string &delim)
{
  bool foundTerminator = false;
  size_t pos = i;

  // Skip over any white space that might come before this group
  while (pos < buffer.length() && isspace(buffer[pos]))
    pos++;

  if (buffer[pos] == '(') {
    if (!extractGroup(target, buffer, pos, "(", ")", false))
      return false;

    // Expect a delimeter just after that
    if (buffer.substr(pos, delim.length()) != delim)
      return false;

    // skip delimeter
    pos += delim.length();

    i = pos;
    return true;
  }

  foundTerminator = false;
  for (; pos < buffer.length(); pos++) {
    if (buffer.substr(pos, delim.length()) == delim) {
      pos += delim.length();
      foundTerminator = true;
      break;
    }

    // Add this character to the contents of the exponent
    target += buffer[pos];
  }

  i = pos;
  return (foundTerminator == true);
}


/**
 * Adds a warning/info/notice message to the log for later perusal.
 */
void MathInterpreter::addMessage (const MathInterpreterMsg::Category category,
				  const MathInterpreterMsg::Code msgCode,
				  const std::string &msg)
{
  assert (m_pCurLine != NULL);

  MathInterpreterMsg message(category,
			     msgCode,
			     m_pCurLine->getFilename(),
			     m_pCurLine->getStartLineNumber(),
			     m_pCurLine->getEndLineNumber(),
			     msg);

  m_messages.push_back(message);
  LOG_INFO << "MSG: " << message;
}


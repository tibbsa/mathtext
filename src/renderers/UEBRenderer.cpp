/**
 * @file UEBRenderer.cpp
 * Unified English Braille rendering engine
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <string>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/scoped_array.hpp>

#include "liblouis-mt.h"
#include "logging.h"
#include "UEBRenderer.h"
#include "MathExceptions.h"

namespace ba = boost::assign;
#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

UEBRenderer::UEBRenderer() : MathRenderer()
{
  internalRenderCount = 0;

  status.isInTextBlock = false;
  status.isNumericMode = false;
  status.isStart = true;
  status.isUsingSpacedOperators = false;
  status.skipFollowingWhitespace = false;

  maxLineLength = UEB_DEFAULT_LINE_LEN;

  lou_setDataPath(".");
}

UEBRenderer::~UEBRenderer()
{
  lou_free();
}

void UEBRenderer::getInterpreterCommandList (std::vector<std::string> &cmdlist)
{
  cmdlist.push_back("SpaceUEBOperators");
}

std::string UEBRenderer::renderDocument (const MathDocument &document)
{
  std::string output;
  bool isInTextBlock = false;

  // We hard code the lengths here for efficiency, but make sure
  // the word wrapping indicators haven't been changed.
  assert (std::string(UEB_WORDWRAP_PRI1).length() == UEB_WORDWRAP_INDLEN);
  assert (std::string(UEB_WORDWRAP_PRI2).length() == UEB_WORDWRAP_INDLEN);
  assert (std::string(UEB_WORDWRAP_PRI3).length() == UEB_WORDWRAP_INDLEN);

  LOG_TRACE << ">> UEBRenderer::renderDocument begin";
  logIncreaseIndent();

  std::string renderedBraille = MathRenderer::renderDocument (document);

  if (maxLineLength) {
    size_t pos = 0;
    while (pos < renderedBraille.length()) {
      std::string curLine;
      std::string curOutputLine;

      size_t eolPos = renderedBraille.find("\n", pos);
      if (eolPos == std::string::npos) { // no final end of line found?
	curLine = renderedBraille.substr(pos);
	// Advance to the end of the buffer
	pos = renderedBraille.length();
      }
      else {
	curLine = renderedBraille.substr(pos, eolPos - pos);
	// Advance to the end of the current line
	pos = eolPos+1;
      }

#ifdef UEB_WRAPPING_DEBUG
      LOG_TRACE << "pos " << pos << "/" << renderedBraille.length() << ", wordwrapping " << curLine.length() << ":'" << stripWrappingIndicators(curLine) << "'";
      logIncreaseIndent();
      LOG_TRACE << curLine;
#endif

      size_t last_break_position [3] = {0, 0, 0};
      size_t curOutputLinePos = 0;
      size_t curOutputLineLength = 0;
      size_t i = 0;

      if (strBeginsWith (curLine, UEB_MATH_BLOCK_BEGIN)) {
#ifdef UEB_WRAPPING_DEBUG
	LOG_TRACE << "beginning math block and indentation...";
#endif

	isInTextBlock = false;
	i = std::string(UEB_MATH_BLOCK_BEGIN).length();
      } else if (strBeginsWith (curLine, UEB_TEXT_BLOCK_BEGIN)) {
#ifdef UEB_WRAPPING_DEBUG
	LOG_TRACE << "beginning text block and disabling indentation...";
#endif

	isInTextBlock = true;
	i = std::string(UEB_TEXT_BLOCK_BEGIN).length();
      }

      while (i < curLine.length()) {
#ifdef UEB_WRAPPING_DEBUG
	LOG_TRACE << "  wrap lookahead @ " << i << ": " << curLine.substr(i, 10);
#endif

	if (curLine.substr(i, UEB_WORDWRAP_INDLEN) == UEB_WORDWRAP_PRI1) {
	  last_break_position [0] = curOutputLinePos;
	  i += UEB_WORDWRAP_INDLEN;
	  continue;
	}
	if (curLine.substr(i, UEB_WORDWRAP_INDLEN) == UEB_WORDWRAP_PRI2) {
	  last_break_position [1] = curOutputLinePos;
	  i += UEB_WORDWRAP_INDLEN;
	  continue;
	}
	if (curLine.substr(i, UEB_WORDWRAP_INDLEN) == UEB_WORDWRAP_PRI3) {
	  last_break_position [2] = curOutputLinePos;
	  i += UEB_WORDWRAP_INDLEN;
	  continue;
	}

	curOutputLine += curLine[i];
	curOutputLinePos++;
	curOutputLineLength++;
	i++;

	/**
	 * We first try to find a "priority 1" break point within the last 20%
	 * of the line. If there is none, then look for priority 2, then
	 * priority 3. If none of those are available, we increase the lookback
	 * up until 50% of the line, at which point we just say to heck with it
	 * and break right where we are.
	 */
	if (curOutputLineLength == maxLineLength) {
#ifdef UEB_WRAPPING_DEBUG
	  LOG_TRACE << "reached maxLength " << curOutputLineLength << "@" << curOutputLinePos;
	  LOG_TRACE << "recorded breakpoints: ";
	  for (int xxxx = 0; xxxx <= 2; xxxx++)
	    LOG_TRACE << " - priority " << (xxxx+1) << ": " << last_break_position[xxxx];
#endif

	  unsigned breakpoint = 0;
	  for (size_t lookback_window_size = (size_t)(maxLineLength/5);
	       !breakpoint && lookback_window_size < (size_t)(maxLineLength/2);
	       lookback_window_size++) {
	    size_t lookback_start_offset = curOutputLine.length() - lookback_window_size;

#ifdef UEB_WRAPPING_DEBUG
	    LOG_TRACE << "lookback window size: " << lookback_window_size << ", starting offset: " << lookback_start_offset;
#endif

	    for (int curPriority = 1; curPriority <= 3 && !breakpoint; curPriority++) {
	      if (last_break_position [curPriority-1] != 0 &&
		  last_break_position [curPriority-1] >= lookback_start_offset) {
#ifdef UEB_WRAPPING_DEBUG
		LOG_TRACE << " - breaking at priority " << curPriority << " position " << last_break_position [curPriority-1];
#endif

		breakpoint = last_break_position [curPriority-1];
	      } else {
#ifdef UEB_WRAPPING_DEBUG
		LOG_TRACE << " - no priotity " << curPriority << " break available";
#endif
	      }
	    }
	  }

	  if (!breakpoint) {
#ifdef UEB_WRAPPING_DEBUG
	    LOG_TRACE << "no suitable breakpoint found: breaking at EOL";
#endif
	    breakpoint = curOutputLinePos;
	  }

	  size_t post_break_length;
	  post_break_length = curOutputLine.length() - breakpoint;
#ifdef UEB_WRAPPING_DEBUG
	  LOG_TRACE << "inserting break at " << breakpoint << ", post-break length: " << post_break_length;
	  LOG_TRACE << "  before: " << curOutputLineLength << ":[" << curOutputLine << "]";
#endif

	  // Delete any whitespace which appears before this point
	  while (breakpoint > 0 && isspace(curOutputLine[breakpoint-1])) {
#ifdef UEB_WRAPPING_DEBUG
	    LOG_TRACE << "  * deleting trailing space before break";
#endif
	    breakpoint--;
	    curOutputLinePos--;
	    curOutputLine.erase(breakpoint, 1);
	  }

	  // Delete spaces following the break
	  while (breakpoint < curOutputLine.length() &&
		 isspace(curOutputLine[breakpoint])) {
#ifdef UEB_WRAPPING_DEBUG
	    LOG_TRACE << "  * deleting space after break";
#endif
	    curOutputLinePos--;
	    curOutputLine.erase(breakpoint, 1);
	  }

	  std::string continuationString;

	  if (isInTextBlock)
	    continuationString = "\n"; // no indentation
	  else
	    continuationString = "\n  "; // 2 cell runover indentation

	  curOutputLine.insert(breakpoint, continuationString);
	  curOutputLinePos = curOutputLine.length();
	  curOutputLineLength = post_break_length + (continuationString.length() - 1);
#ifdef UEB_WRAPPING_DEBUG
	  LOG_TRACE << "  after: " << curOutputLineLength << ":[" << curOutputLine << "]";
#endif

	  // reset breakpoints for new line
	  for (int curPriority = 1; curPriority <= 3; curPriority++)
	    last_break_position[curPriority-1] = 0;
	}
      }

#ifdef UEB_WRAPPING_DEBUG
      LOG_TRACE << "wordwrapped to: " << curOutputLine.length() << ":[" << curOutputLine << "]";
      logDecreaseIndent();
#endif

      output += curOutputLine + "\n";
    }
  }
  else
    output = renderedBraille;

  LOG_TRACE << "<< UEBRenderer::renderDocument end: " << output;
  logDecreaseIndent();

  return output;
}

void UEBRenderer::disableLineWrapping (void)
{
  maxLineLength = 0;
}

void UEBRenderer::enableLineWrapping (const unsigned length)
{
  assert (length != 0);
  maxLineLength = length;
}


std::string UEBRenderer::stripWrappingIndicators (const std::string &input) const
{
  std::string temp = input;
  boost::algorithm::replace_all (temp, UEB_WORDWRAP_PRI1, "");
  boost::algorithm::replace_all (temp, UEB_WORDWRAP_PRI2, "");
  boost::algorithm::replace_all (temp, UEB_WORDWRAP_PRI3, "");
  return temp;
}

void UEBRenderer::beginInternalRender (void)
{
  internalRenderCount++;
  statusStack.push (status);

  LOG_TRACE << "** push render stack #" << internalRenderCount << ", status: #=" << status.isNumericMode << ", S=" << status.isStart << ", spaceOp=" << status.isUsingSpacedOperators;

  logIncreaseIndent();
}

bool UEBRenderer::doingInternalRender (void) const
{
  return (internalRenderCount > 0);
}

void UEBRenderer::endInternalRender (void)
{
  assert (internalRenderCount != 0);
  internalRenderCount--;
  status = statusStack.top();
  statusStack.pop();

  logDecreaseIndent();
  LOG_TRACE << "** pop render stack #" << internalRenderCount << ", status: #=" << status.isNumericMode << ", S=" << status.isStart << ", spaceOp=" << status.isUsingSpacedOperators;
}

std::string UEBRenderer::translateToBraille (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  output = translateBraillePunctuation (s);
  output = translateBrailleLetterIndicators (output);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::translateBrailleLetterIndicators (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  bool inNumericMode = false;

  for (size_t pos = 0; pos < s.length(); pos++) {
    char c = s [pos];

    if (c == '#') {
      inNumericMode = true;
      output += c;
      continue;
    }

    // End numeric mode if we find anything other than "A-F", "4" (period),
    // "2" (comma)
    if (inNumericMode) {
      if (!isOneOf(c, UEB_NUMERIC_MODE_SYMBOLS)) {
	inNumericMode = false;
      }
    } else if (c >= 'A' && c <= 'Z') {
      output += UEB_CAPITAL_SIGN;
    }

    output += c;
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::translateBraillePunctuation (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  for (size_t pos = 0; pos < s.length(); pos++) {
    char c = s [pos];

    if (c == ',') {
      output += UEB_COMMA;
      continue;
    }

    if (c == '.') {
      output += UEB_PERIOD;
      continue;
    }

    output += c;
  }


  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

// Returns true if the provided vector consists of a single 'item', as that
// is defined by the Technical Guidelines, s. 7.1 or 12.1
// (An 'item' is: a number; a fraction; a radical; a narrow; a shape;
//  anything in paranthesees, square brackets, or braces; anything in
//  braille grouping indicators; or the previous character)
bool UEBRenderer::isBrailleItem (const MDEVector &v)
{
  // any vector containing multiple items will be, by definition, not an
  // item
  if (v.empty() || v.size() > 1)
    return false;

  const MathDocumentElementPtr element = v.front();
  const MathDocumentElement *ptr = element.get();

  // Case 1: Positive numbers.  (Negative numbers can't be considered
  // an item because the minus sign is itself an item.)
  if (const MDE_Number *mn = dynamic_cast<const MDE_Number *>(ptr)) {
    if (!mn->isNegative())
      return true;
    else
      return false;
  }

  // Case 2: Fractions, roots and operators are always items in and of
  // themselves.  (See Technical Guidelines, section 7.6.)
  if (dynamic_cast<const MDE_Fraction *>(ptr) ||
      dynamic_cast<const MDE_Root *>(ptr) ||
      dynamic_cast<const MDE_Operator *>(ptr))
    return true;

  // Case 3: Single letters can be an item unto their own
  if (const MDE_MathBlock *mb = dynamic_cast<const MDE_MathBlock *>(ptr)) {
    std::string contents = mb->getText();
    if (contents.length() == 1 && isalpha(contents[0])) {
      return true;
    } else {
      return false;
    }
  }

  return false;
}

// Returns true if the provided fraction is a "simple" fraction -- that is,
// both the numerator and the denominator contain only a simple number.
bool UEBRenderer::isSimpleFraction (const MDE_Fraction &frac)
{
  if (frac.getNumerator().size() > 1 || frac.getDenominator().size() > 1)
    return false;

  if (!dynamic_cast<const MDE_Number *>(frac.getNumerator().front().get()))
    return false;

  if (!dynamic_cast<const MDE_Number *>(frac.getDenominator().front().get()))
    return false;

  return true;
}

// called to output what is known to be 'math' material. takes care of
// any necessary mode switches, etc.,
std::string UEBRenderer::renderMathContent (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  // If we're in numeric mode, we might need a letter indicator here.
  if (status.isNumericMode) {
    if (isOneOf(s[0], "abcdefghij")) {
      LOG_TRACE << "- inserting letter indicator before string b/c in math mode";
      output = UEB_G1;
      status.isNumericMode = false;
    }
  }

  if (status.skipFollowingWhitespace) {
    output += boost::trim_left_copy(s);
    status.skipFollowingWhitespace = false;
  }
  else
    output += s;

  status.isStart = false;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::renderTextContent (const std::string &s)
{
  std::string output;
  std::string braille_string;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  // Assume that the braille could grow to be a bit larger than our max
  // LibLouis buffer size.  If this will likely be a problem, abort.
  //## TODO: Split long strings into ~500-byte chunks (using a "period space"
  // or just a space as a likely split point) and feed to LibLouis in smaller
  // sections to get around this limitation.
  if (s.length() >= (0.90 * LIBLOUIS_MAXSTRING)) {
    LOG_ERROR << "! Braille translation max buffer size error on '" << s << "'!";
    std::ostringstream os;
    os << "This text is too long to translate to braille, and a workaround has not yet been implemented: '" << s << "'";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  boost::scoped_array<ll_widechar> input_buffer (new ll_widechar [LIBLOUIS_MAXSTRING]);
  boost::scoped_array<ll_widechar> braille_buffer (new ll_widechar [LIBLOUIS_MAXSTRING]);

  unsigned inlen;
  strToWideCharString(s, input_buffer.get(), inlen);
  unsigned outlen = LIBLOUIS_MAXSTRING;

  LOG_TRACE << "Sending " << inlen << " chars to louis using table '" << LIBLOUIS_UEB_G1_TABLE << "': {" << s << "}, max output size=" << outlen;
  if (!lou_translateString(LIBLOUIS_UEB_G1_TABLE,
			   input_buffer.get(),
			   (int *)&inlen,
			   braille_buffer.get(),
			   (int *)&outlen,
			   NULL /* no typeform checking */,
			   NULL /* don't care about spacing info */,
			   0 /* no modes set */)) {
    LOG_ERROR << "! Braille translation error on '" << s << "'!";
    std::ostringstream os;
    lou_logEnd();
    os << "Error occurred while trying to render braille for text: '" << s << "'";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  wideCharStringToStr(braille_buffer.get(), outlen, braille_string);
  LOG_TRACE << "Louis returned " << outlen << " chars: {" << braille_string << "}";
  
  // Insert wrapping indicators.
  if (maxLineLength) {
    boost::replace_all(braille_string, "4 ", "4 " UEB_WORDWRAP_PRI1);
    boost::replace_all(braille_string, "6 ", "6 " UEB_WORDWRAP_PRI1);
    boost::replace_all(braille_string, "8 ", "8 " UEB_WORDWRAP_PRI1);
    boost::replace_all(braille_string, " ", " " UEB_WORDWRAP_PRI2);
    boost::replace_all(braille_string, UEB_LEFT_PAREN, UEB_WORDWRAP_PRI1 UEB_LEFT_PAREN);
    boost::replace_all(braille_string, UEB_LEFT_BRACKET, UEB_WORDWRAP_PRI1 UEB_LEFT_BRACKET);
    boost::replace_all(braille_string, UEB_LEFT_BRACE, UEB_WORDWRAP_PRI1 UEB_LEFT_BRACE);
  }

  //## There is a bug in liblouis that results in single letters
  //   getting letter indicators before them unnecessarily.  Fix this.
  std::string fixed_braille_string;
  fixed_braille_string = braille_string;
  for (char ch = 'a'; ch <= 'z'; ch++) {
    const std::string searchStr = boost::str(boost::format(";%c ") % ch);
    const std::string replaceStr = boost::str(boost::format("%c ") % ch);

    boost::ireplace_all(fixed_braille_string, searchStr, replaceStr);
  }

  output += fixed_braille_string;

  status.isNumericMode = false;
  status.isStart = false;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::renderSourceLine (const MDE_SourceLine *e)
{
  std::string output;

  LOG_TRACE << "%% " << e->getString();

  return output;
}

std::string UEBRenderer::renderCommand (const MDE_Command *e)
{
  std::string output;

  LOG_TRACE << "%% COMMAND: " << e->getString();
  if (boost::iequals(e->getName(), "SpaceUEBOperators")) {
    status.isUsingSpacedOperators = (e->getParameters() == "true");
    LOG_TRACE << "- extra operator spacing: " << status.isUsingSpacedOperators;
  }

  return output;
}

std::string UEBRenderer::renderMathModeMarker (const MDE_MathModeMarker *e)
{
  std::string output;

  if (e->getType() == MDE_MathModeMarker::BLOCK_MARKER) {
    // We should only see the start of a math "block" if we are also at the
    // start of a line.  We can't commence a math block mid-line, as the
    // indenting and word wrapping requirements will be different.
    assert (status.isStart == true);
    status.isInTextBlock = false;
    output += UEB_MATH_BLOCK_BEGIN;
    LOG_TRACE << "* commencing math block mode";
  }

  return output;
}

std::string UEBRenderer::renderTextModeMarker (const MDE_TextModeMarker *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (e->getType() == MDE_TextModeMarker::BLOCK_MARKER) {
    // We should only see the start of a text "block" if we are also at the
    // start of a line.  We can't commence a text block mid-line, as the
    // indenting and word wrapping requirements will be different.
    assert (status.isStart == true);
    status.isInTextBlock = true;
    output += UEB_TEXT_BLOCK_BEGIN;
    LOG_TRACE << "* commencing text block mode";
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::renderLineBreak (const MDE_LineBreak *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output += "\n";
  status.isNumericMode = false;
  status.isStart = true;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::renderTextBlock (const MDE_TextBlock *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (!status.isStart)
    output = " ";

  output += renderTextContent(e->getText());
  status.isStart = false;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderMathBlock (const MDE_MathBlock *e)
{
  std::string output;
  std::string brl;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  brl = translateToBraille(e->getText());
  output = renderMathContent(brl);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output = renderTextContent(e->getText() + " ");

  // Drop letter indicator if one has appeared
  if (output.substr(0, 1) == UEB_G1)
    output.erase(output.begin(), output.begin()+1);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderGroup (const MDE_Group *e)
{
  std::string renderedContents;
  std::string openChar, closeChar;
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  switch (e->getType()) {
  case MDE_Group::PARENTHESES:
    openChar = UEB_LEFT_PAREN;
    closeChar = UEB_RIGHT_PAREN;
    break;

  case MDE_Group::BRACKETS:
    openChar = UEB_LEFT_BRACKET;
    closeChar = UEB_RIGHT_BRACKET;
    break;

  case MDE_Group::BRACES:
    openChar = UEB_LEFT_BRACE;
    closeChar = UEB_RIGHT_BRACE;
    break;

  default:
    assert(0);
  }


  beginInternalRender();
  status.isNumericMode = false;
  renderedContents = renderVector (e->getContents());
  endInternalRender();

  if (maxLineLength)
    output = UEB_WORDWRAP_PRI2;

  output += openChar;
  output += boost::trim_copy(renderedContents);
  output += closeChar;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderNumber (const MDE_Number *e)
{
  std::string output;
  std::string printNumber;
  std::string brailleNumber;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (maxLineLength)
    brailleNumber = UEB_WORDWRAP_PRI3;

  size_t pos = 0;
  printNumber = e->getStandardNotation();
  if (printNumber[pos] == '-') {
    brailleNumber += "-";
    pos++;
  }

  brailleNumber += UEB_NUMBER_SIGN;

  // If we are already in numeric mode, and the next character is not
  // a number but could be confused as such, add a grade 1 terminator.
  while (pos < printNumber.length()) {
    char c = printNumber[pos];

    // We should only see "0-9", ".", "," and a space here.
    assert (isdigit(c) || (c == '.') || (c == ',') || (c == ' '));

    if (isdigit(c)) {
      // In braille, numbers 1-9(0) are represented by letters A-J.  On the
      // ASCII tables, numbers 1-9 are code 49-57, and number 0 is code 48.
      // The beginning of the alphabet (uppercase) is at #65, so 1=A=65, etc.
      // In other words, adding 16 to the digit will get the uppercase letter.
      brailleNumber += (c == '0')
	? "J"
	: boost::str(boost::format("%c") % (char)(16 + (int)c));
    } else if (c == '.') {
      brailleNumber += UEB_PERIOD;
    } else if (c == ',') {
      brailleNumber += UEB_COMMA;
    } else if (c == ' ') {
      brailleNumber += UEB_NUMERIC_SPACE;
    } else {
      assert(0);
    }

    pos++;
  }

  output = renderMathContent(brailleNumber);
  status.isNumericMode = true;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderOperator (const MDE_Operator *e)
{
  std::string output;

  static std::map<MDE_Operator::Operator,std::string> opmap = ba::map_list_of
    ( MDE_Operator::ADDITION, UEB_PLUS_SIGN )
    ( MDE_Operator::SUBTRACTION, UEB_MINUS_SIGN )
    ( MDE_Operator::DIVISION, UEB_DIV_SIGN )
    ( MDE_Operator::MULTIPLICATION, UEB_TIMES_SIGN )
    ;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (!opmap.count(e->getOperator())) {
    LOG_ERROR << "! Operator translation error - unknown operator on " << (*e);
    std::ostringstream os;
    os << "The braille generator encountered a mathematical operator it does not understand. Please contact technical support for assistance. (Operator: " << (*e) << ")";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  if (status.isUsingSpacedOperators)
    output = " ";

  if (maxLineLength)
    output += UEB_WORDWRAP_PRI2;

  output += opmap[e->getOperator()];

  if (status.isUsingSpacedOperators)
    output += " ";

  status.skipFollowingWhitespace = true;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderComparator (const MDE_Comparator *e)
{
  std::string output;

  static std::map<MDE_Comparator::Comparator,std::string> compmap = ba::map_list_of
    ( MDE_Comparator::LESS_THAN, UEB_LESS_THAN )
    ( MDE_Comparator::GREATER_THAN, UEB_GREATER_THAN )
    ( MDE_Comparator::EQUALS, UEB_EQUAL_SIGN )
    ( MDE_Comparator::APPROX_EQUALS, UEB_APPROX_EQUAL )
    ( MDE_Comparator::NOT_EQUALS, UEB_UNEQUAL_SIGN )
    ( MDE_Comparator::GREATER_THAN_EQUALS, UEB_GREATER_THAN_EQ )
    ( MDE_Comparator::LESS_THAN_EQUALS, UEB_LESS_THAN_EQ )
    ;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (!compmap.count(e->getComparator())) {
    LOG_ERROR << "! Comparator translation error - unknown comparator on " << (*e);
    std::ostringstream os;
    os << "The braille generator encountered a mathematical comparator it does not understand. Please contact technical support for assistance. (Comparator: " << (*e) << ")";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  if (maxLineLength)
    output = " " UEB_WORDWRAP_PRI1 + compmap [e->getComparator()] + " ";
  else
    output = " " + compmap [e->getComparator()] + " ";

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderGreekLetter (const MDE_GreekLetter *e)
{
  std::string output;

#define MAPLC(ltr, brl) (MDE_GreekLetter::ltr, UEB_GREEK_SIGN brl)
#define MAPUC(ltr, brl) (MDE_GreekLetter::ltr, UEB_CAPITAL_SIGN UEB_GREEK_SIGN brl)
#define MAP(lcl, ucl, brl) MAPLC(lcl, brl) MAPUC(ucl, brl)

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  static std::map<MDE_GreekLetter::Character,std::string> charmap = ba::map_list_of
    MAP(alpha, Alpha, UEB_GREEK_ALPHA)
    MAP(beta, Beta, UEB_GREEK_BETA)
    MAP(gamma, Gamma, UEB_GREEK_GAMMA)
    MAP(delta, Delta, UEB_GREEK_DELTA)
    MAP(epsilon, Epsilon, UEB_GREEK_EPSILON)
    MAP(zeta, Zeta, UEB_GREEK_ZETA)
    MAP(eta, Eta, UEB_GREEK_ETA)
    MAP(theta, Theta, UEB_GREEK_THETA)
    MAP(iota, Iota, UEB_GREEK_IOTA)
    MAP(kappa, Kappa, UEB_GREEK_KAPPA)
    MAP(lambda, Lambda, UEB_GREEK_LAMBDA)
    MAP(mu, Mu, UEB_GREEK_MU)
    MAP(nu, Nu, UEB_GREEK_NU)
    MAP(xi, Xi, UEB_GREEK_XI)
    MAP(omicron, Omicron, UEB_GREEK_OMICRON)
    MAP(pi, Pi, UEB_GREEK_PI)
    MAP(rho, Rho, UEB_GREEK_RHO)
    MAP(sigma, Sigma, UEB_GREEK_SIGMA)
    MAP(tau, Tau, UEB_GREEK_TAU)
    MAP(upsilon, Upsilon, UEB_GREEK_UPSILON)
    MAP(phi, Phi, UEB_GREEK_PHI)
    MAP(chi, Chi, UEB_GREEK_CHI)
    MAP(psi, Psi, UEB_GREEK_PSI)
    MAP(omega, Omega, UEB_GREEK_OMEGA)
    ;

#undef MAP
#undef MAPUC
#undef MAPLC

  if (!charmap.count(e->getValue())) {
    LOG_ERROR << "! Greek translation error - unknown character on " << (*e);
    std::ostringstream os;
    os << "The braille generator encountered a Greek letter it does not understand. Please contact technical support for assistance. (Letter: " << (*e) << ")";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  if (maxLineLength)
    output = UEB_WORDWRAP_PRI3;

  output += renderMathContent(charmap [e->getValue()]);
  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";

  return output;
}

std::string UEBRenderer::renderSymbol (const MDE_Symbol *e)
{
  std::string output;

  static std::map<MDE_Symbol::Symbol,std::string> symmap = ba::map_list_of
    ( MDE_Symbol::COMMA, UEB_COMMA)
    ( MDE_Symbol::CURRENCY_CENTS, UEB_CURRENCY_CENTS )
    ( MDE_Symbol::CURRENCY_EURO, UEB_CURRENCY_EURO )
    ( MDE_Symbol::CURRENCY_FRANC, UEB_CURRENCY_FRANC )
    ( MDE_Symbol::CURRENCY_POUND, UEB_CURRENCY_POUND )
    ( MDE_Symbol::CURRENCY_DOLLAR, UEB_CURRENCY_DOLLAR )
    ( MDE_Symbol::CURRENCY_YEN, UEB_CURRENCY_YEN )
    ( MDE_Symbol::FACTORIAL, UEB_FACTORIAL )
    ( MDE_Symbol::LEFT_BRACE, UEB_LEFT_BRACE )
    ( MDE_Symbol::LEFT_BRACKET, UEB_LEFT_BRACKET )
    ( MDE_Symbol::LEFT_PAREN, UEB_LEFT_PAREN )
    ( MDE_Symbol::PERCENT, UEB_PERCENT )
    ( MDE_Symbol::PERIOD, UEB_PERIOD )
    ( MDE_Symbol::RIGHT_BRACE, UEB_RIGHT_BRACE )
    ( MDE_Symbol::RIGHT_BRACKET, UEB_RIGHT_BRACKET )
    ( MDE_Symbol::RIGHT_PAREN, UEB_RIGHT_PAREN )
    ( MDE_Symbol::THEREFORE, UEB_THEREFORE )
    ;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (!symmap.count(e->getSymbol())) {
    LOG_ERROR << "! Symbol translation error - unknown symbol on " << (*e);
    std::ostringstream os;
    os << "The braille generator encountered a mathematical symbol it does not understand. Please contact technical support for assistance. (Symbol: " << (*e) << ")";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  output = renderMathContent(symmap [e->getSymbol()]);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;

}

std::string UEBRenderer::renderModifier (const MDE_Modifier *e)
{
  std::string renderedArgument;
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  beginInternalRender();
  status.isNumericMode = false;
  renderedArgument = renderVector(e->getArgument());
  endInternalRender();

  // Include grouping indicators only if the symbol to be modified is
  // something more than an 'item'.  See isBrailleItem() for details.
  if (maxLineLength)
    output = UEB_WORDWRAP_PRI3;

  if (!isBrailleItem(e->getArgument()))
    output += UEB_GROUP_BEGIN + renderedArgument + UEB_GROUP_END;
  else
    output += renderedArgument;

  switch (e->getModifier()) {
  case MDE_Modifier::OVER_ARROW_RIGHT:
    output += UEB_OVER_ARROW_RIGHT;
    break;

  case MDE_Modifier::OVER_BAR:
    output += UEB_OVER_BAR;
    break;

  case MDE_Modifier::OVER_HAT:
    output += UEB_OVER_HAT;
    break;

  default:
    assert(0);
  }

  output = renderMathContent(output);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderRoot (const MDE_Root *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (maxLineLength)
    output = UEB_WORDWRAP_PRI3;

  if (e->getIndex().empty()) { /* simple square root */
    std::string renderedRootArgument;

    beginInternalRender();
    status.isNumericMode = false;
    renderedRootArgument = renderVector(e->getArgument());
    endInternalRender();

    LOG_TRACE << "- rendered root: " << renderedRootArgument;

    output += renderMathContent(UEB_ROOT_BEGIN + renderedRootArgument + UEB_ROOT_END);
  } else {

    // Insert the index as an exponent at the start of the root argment, e.g.
    // _/3(8) becomes [open root] [exponent] #c h [end root]
    std::string renderedRootIndex, renderedRootArgument;

    MathDocumentElementPtr index_exponent;
    index_exponent = boost::make_shared<MDE_Exponent>(e->getIndex());

    beginInternalRender();
    status.isNumericMode = false;
    renderedRootIndex = renderElement(index_exponent.get());

    status.isNumericMode = false;
    renderedRootArgument = renderVector(e->getArgument());
    endInternalRender();

    LOG_TRACE << "- rendered index: " << renderedRootIndex;
    LOG_TRACE << "- rendered root: " << renderedRootArgument;

    output += renderMathContent(UEB_ROOT_BEGIN + renderedRootIndex + renderedRootArgument + UEB_ROOT_END);
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderSummation (const MDE_Summation *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (maxLineLength)
    output = UEB_WORDWRAP_PRI3;

  output += UEB_CAPITAL_SIGN UEB_GREEK_SIGN UEB_GREEK_SIGMA;

  if (!e->getLowerBound().empty()) {
    std::string renderedBound;
    bool savedNumericMode;

    beginInternalRender();
    status.isNumericMode = false;
    renderedBound = renderVector(e->getLowerBound());
    savedNumericMode = status.isNumericMode;
    endInternalRender();

    LOG_TRACE << "- rendered lower bound: " << renderedBound << " (numeric mode: " << savedNumericMode << ")";

    if (isBrailleItem(e->getLowerBound())) {
      output += renderMathContent(boost::str(boost::format(UEB_DIRECTLY_BELOW "%s") % renderedBound));

      // Restore numeric mode status for "item" bounds, in case we need to
      // follow this with a letter indicator (e.g. a sigma with only a lower
      // bound that is a number, and is then followed by an 'a')
      status.isNumericMode = savedNumericMode;
    } else {
      output += renderMathContent(boost::str(boost::format(UEB_DIRECTLY_BELOW UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedBound));
    }
  }

  if (!e->getUpperBound().empty()) {
    std::string renderedBound;
    bool savedNumericMode;

    beginInternalRender();
    status.isNumericMode = false;
    renderedBound = renderVector(e->getUpperBound());
    savedNumericMode = status.isNumericMode;
    endInternalRender();

    LOG_TRACE << "- rendered upper bound: " << renderedBound << " (numeric mode: " << savedNumericMode << ")";

    if (isBrailleItem(e->getUpperBound())) {
      output += renderMathContent(boost::str(boost::format(UEB_DIRECTLY_ABOVE "%s") % renderedBound));

      // Restore numeric mode status for "item" bounds, in case we need to
      // follow this with a letter indicator (e.g. a sigma with an upper
      // bound that is a number, and is then followed by an 'a')
      status.isNumericMode = savedNumericMode;
    } else {
      output += renderMathContent(boost::str(boost::format(UEB_DIRECTLY_ABOVE UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedBound));
    }
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderFraction (const MDE_Fraction *e)
{
  std::string renderedNumerator, renderedDenominator;
  std::string output;
  bool simpleFraction = isSimpleFraction(*e);

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  beginInternalRender();
  status.isNumericMode = false;
  renderedNumerator = renderVector (e->getNumerator());

  status.isNumericMode = false;
  renderedDenominator = renderVector (e->getDenominator());
  endInternalRender();

  LOG_TRACE << "- rendered numerator:   " << renderedNumerator;
  LOG_TRACE << "- rendered denominator: " << renderedDenominator;
  LOG_TRACE << "- is simple fraction? " << simpleFraction;

  if (maxLineLength)
    output = UEB_WORDWRAP_PRI2;

  if (simpleFraction) {
    // Do not permit word wrapping within simple fractions
    if (maxLineLength) {
      renderedNumerator = stripWrappingIndicators(renderedNumerator);
      renderedDenominator = stripWrappingIndicators(renderedDenominator);
    }

    // The dividing slash does not cancel numeric mode, so remove the
    // extra number sign that will appear in the denominator
    renderedDenominator.erase (0, 1);

    output += renderMathContent(boost::str(boost::format("%s" UEB_SIMPLE_FRAC_DIVIDER "%s") % renderedNumerator % renderedDenominator));
  } else {
    if (maxLineLength)
      output += renderMathContent(boost::str(boost::format(UEB_FRAC_BEGIN "%s" UEB_FRAC_DIVIDER UEB_WORDWRAP_PRI3 "%s" UEB_FRAC_END) % renderedNumerator % renderedDenominator));
    else
      output += renderMathContent(boost::str(boost::format(UEB_FRAC_BEGIN "%s" UEB_FRAC_DIVIDER "%s" UEB_FRAC_END) % renderedNumerator % renderedDenominator));
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderExponent (const MDE_Exponent *e)
{
  std::string renderedExponent;
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  beginInternalRender();
  status.isNumericMode = false;
  renderedExponent = renderVector (e->getValue());

  // Store and save numeric mode status in case we might need a letter
  // indicator later.
  bool endedInNumericMode = status.isNumericMode;
  endInternalRender();

  // Insert grouping symbols only if the exponent contents is not an 'item'
  // (See isBrailleItem() for details)
  if (isBrailleItem(e->getValue())) {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_UP "%s") % renderedExponent));
    status.isNumericMode = endedInNumericMode;
  } else {
    if (maxLineLength)
      output = renderMathContent(boost::str(boost::format(UEB_LEVEL_UP UEB_WORDWRAP_PRI3 UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedExponent));
    else
      output = renderMathContent(boost::str(boost::format(UEB_LEVEL_UP UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedExponent));
    status.isNumericMode = false;
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}

std::string UEBRenderer::renderSubscript (const MDE_Subscript *e)
{
  std::string renderedSubscript;
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  beginInternalRender();
  status.isNumericMode = false;
  renderedSubscript = renderVector (e->getValue());

  // Store and save numeric mode status in case we might need a letter
  // indicator later.
  bool endedInNumericMode = status.isNumericMode;
  endInternalRender();

  // Insert grouping symbols only if the subscript contents is not an 'item'
  // (See isBrailleItem() for details)
  if (isBrailleItem(e->getValue())) {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN "%s") % renderedSubscript));
    status.isNumericMode = endedInNumericMode;
  } else {
    if (maxLineLength)
      output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN UEB_WORDWRAP_PRI3 UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedSubscript));
    else
      output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedSubscript));

    status.isNumericMode = false;
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << stripWrappingIndicators(output) << ")";
  return output;
}


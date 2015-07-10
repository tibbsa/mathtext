/**
 * @file MathDocument.cpp
 * Implementation of the container holding the interpreted source document
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <cctype>
#include <map>
#include <assert.h>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include "logging.h"
#include "MathDocument.h"
#include "MathExceptions.h"

namespace ba = boost::algorithm;

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
  inTextBlock = false;
  textBlockBeganLine = 0;
  mathBlockBeganLine = 0;

  for (std::vector<MathDocumentLine>::const_iterator it = src.m_document.begin();
       it != src.m_document.end(); ++it) {

    m_document.push_back (boost::make_shared<MDE_SourceLine>(it->getFilename(),
							     it->getStartLineNumber(), 
							     it->getEndLineNumber(), 
							     it->getContent()));
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
    (MDM_SUSPECT_MATH_IN_TEXT, "Suspected math symbols in a text passage")
    (MDM_SUSPECT_FRACTION, "Suspect missing open fraction symbol (@)")
    (MDM_UNKNOWN_GREEK, "Unknown Greek character symbol")
    (MDM_FRACTION_NOT_TERMINATED, "Fraction terminator symbol (#) appears to be missing")
    (MDM_EXPONENT_NOT_TERMINATED, "Exponent begins with opening paren '(' but is never terminated with a closing paren ')'")
    (MDM_SUBSCRIPT_NOT_TERMINATED, "Subscript begins with opening paren '(' but is never terminated with a closing paren ')'")
    (MDM_ROOT_INDEX_NOT_TERMINATED, "Root includes a complex index with opening bracket '[' but is never terminated with a closing bracket ']'")
    (MDM_ROOT_NOT_TERMINATED, "Root begins with opening paren '(' but is never terminated with a closing paren ')'")

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
  isStartOfLine = true;

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

#define ATTEMPT(class) { if (interpret##class (temp_elements, buffer, i)) { PUSH_CATCH_BUFFER; elements.insert (elements.end(), temp_elements.begin(), temp_elements.end()); goto NextChar; } }

MDEVector MathDocument::interpretBuffer (const std::string &buffer)
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
    LOG_TRACE << "At pos " << i << ", char '" << c << "', " << (inTextBlock ? "T" : "M") << (inTextMode ? "t" : "m");
    logIncreaseIndent();
 
    /**
     * Mode changes: text to math or math to text. Dump what we have up to 
     * this point whenever there is a mode change.
     */
    if (c == '$') {
      if (inTextMode) {
	LOG_TRACE << "* entering math mode; pushing '" << catch_buffer << "'";
	PUSH_CATCH_BUFFER;
	inTextMode = false;
	elements.push_back (boost::make_shared<MDE_MathModeMarker>());
      } else {
	LOG_TRACE << "! attempt to enter math mode while in math mode";
	MSG_WARNINGX(MDM_NESTED_MATH_MODE);
      }

      goto AdvanceNextChar;
    }

    if (c == '&') {
      if (!inTextMode) {
	LOG_TRACE << "* entering text mode; pushing '" << catch_buffer << "'";
	PUSH_CATCH_BUFFER;
	inTextMode = true;
	elements.push_back (boost::make_shared<MDE_TextModeMarker>());
      } else {
	LOG_TRACE << "! attempt to enter math mode while in text mode";
	MSG_WARNINGX(MDM_NESTED_TEXT_MODE);
      }

      goto AdvanceNextChar;
    }

    // If this is the first thing we're seeing on the line, check to see
    // if the first blob appears to be an 'item number' (as might appear 
    // in homework).
    if (isStartOfLine && !inTextMode) {
      ATTEMPT(ItemNumber);
      isStartOfLine = false;
    }

    if (inTextMode)
      goto HandleTextBlocks;

    ATTEMPT(Operator);
    ATTEMPT(Comparator);
    ATTEMPT(GreekLetter);
    ATTEMPT(Symbol);
    ATTEMPT(Fraction);
    ATTEMPT(Root); // do this before a subscript to avoid confusion
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
 * Attempts to interpret the specified characters as item numbers. THese 
 * may appear at the start of the line.  We treat them as a special text 
 * block to allow rendering engines to distinguish question numbers as 
 * they see fit.
 *
 * Accepted formats:
 * "1. " "a. "
 * "1) " "a) "
 * "(1) " "(a) "
 */
bool MathDocument::interpretItemNumber (MDEVector &target,
					const std::string &src, 
					size_t &i)
{
  const std::vector<std::string> patterns = boost::assign::list_of 
    ( std::string("^(\\d{1,4}\\.\\s+)") ) // 1. 
    ( std::string("^([A-Za-z]{1}\\.\\s+)") ) // a. 
    ( std::string("^(\\d{1,4}\\)\\s+)") ) // 1)
    ( std::string("^([A-Za-z]{1}\\)\\s+)") ) // a) 
    ( std::string("^(\\(\\d{1,4}\\)\\s+)") ) // (1)
    ( std::string("^(\\([A-Za-z]{1}\\)\\s+)") ) // (a)
    ;

  for (std::vector<std::string>::const_iterator it = patterns.begin();
       it != patterns.end(); ++it) {
    const std::string pattern = *it;

    boost::regex exp(pattern);
    boost::smatch mr;
    if (boost::regex_search(src.substr(i), mr, exp)) {
      std::string item_number = mr[1];
      ba::trim(item_number);
      
      LOG_TRACE << "* added item number '" << item_number << "' with pattern '" << pattern << "'";
      i = i + mr.length();
      target.push_back (boost::make_shared<MDE_ItemNumber>(item_number));
      return true;
    }
  }

  return false;
}

/**
 * Attempts to interpret the specified characters as signs of operation
 * (+, -, / *)
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
bool MathDocument::interpretOperator (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  const std::string temp = src.substr(i, 3);

  if (temp != " / " && !isOneOf(temp[0], "+*-"))
    return false;

  // +/- is a different symbol and ought not be handled here
  if (temp == "+/-" || temp == "-/+")
    return false;

  if (temp[0] == '+') {
    LOG_TRACE << "* adding addition sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::ADDITION));
    i++;
  } else if (temp[0] == '*') {
    LOG_TRACE << "* adding multiplication sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::MULTIPLICATION));
    i++;
  } else if (temp[0] == '-') {
    LOG_TRACE << "* adding subtraction sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::SUBTRACTION));
    i++;
  }
  else if (temp == " / ") {
    LOG_TRACE << "* adding division sign";
    target.push_back (boost::make_shared<MDE_Operator>(MDE_Operator::DIVISION));
    i += 3;
  }
  else
    assert (false); // Tests above should prevent us from getting here

  return true;
}

/**
 * Attempts to interpret the specified characters as signs of comparison
 * (< > = != ~= <= >=)
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
struct ComparatorSymbolMap { 
  ComparatorSymbolMap (const std::string &s, const MDE_Comparator::Comparator c) : search(s), symbol(c) {}
  std::string search;
  MDE_Comparator::Comparator symbol;
};

bool MathDocument::interpretComparator (MDEVector &target,
					const std::string &src, 
					size_t &i)
{
  static const std::vector<ComparatorSymbolMap> map = boost::assign::list_of 
    ( ComparatorSymbolMap("<=", MDE_Comparator::LESS_THAN_EQUALS) )
    ( ComparatorSymbolMap(">=", MDE_Comparator::GREATER_THAN_EQUALS) )
    ( ComparatorSymbolMap("!=", MDE_Comparator::NOT_EQUALS) )
    ( ComparatorSymbolMap("~=", MDE_Comparator::APPROX_EQUALS) )
    ( ComparatorSymbolMap("<", MDE_Comparator::LESS_THAN) )
    ( ComparatorSymbolMap(">", MDE_Comparator::GREATER_THAN) )
    ( ComparatorSymbolMap("=", MDE_Comparator::EQUALS) );

  const std::string temp = src.substr(i, 2);

  for (std::vector<ComparatorSymbolMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const ComparatorSymbolMap &mi = *it;
    // Look for the first character
    if (temp[0] == mi.search[0]) {
      // It matches. If the second one also has to match, consider it too.
      if (mi.search.length() == 1 || temp[1] == mi.search[1]) {
	LOG_TRACE << "* added comparator sign (" << mi.search << ")";
	target.push_back (boost::make_shared<MDE_Comparator>(mi.symbol));
	i += mi.search.length();
	return true;
      }
    }
  }

  return false;
}

/**
 * Attempts to interpret a greek letter: %a, %b, etc.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
struct GreekLetterMap { 
  GreekLetterMap (const char s, const MDE_GreekLetter::Character c) : symbol(s), character(c) {}
  char symbol;
  MDE_GreekLetter::Character character;
};
#define CHARMAP(symbol1,char1,symbol2,char2) ( GreekLetterMap(symbol1, MDE_GreekLetter::char1) ) ( GreekLetterMap(symbol2, MDE_GreekLetter::char2) )
bool MathDocument::interpretGreekLetter (MDEVector &target,
					 const std::string &src, 
					 size_t &i)
{

  static const std::vector<GreekLetterMap> map = boost::assign::list_of 
    CHARMAP('a', alpha, 'A', Alpha)
    CHARMAP('b', beta, 'B', Beta)
    CHARMAP('g', gamma, 'G', Gamma)
    CHARMAP('d', delta, 'D', Delta)
    CHARMAP('e', epsilon, 'E', Epsilon)
    CHARMAP('z', zeta, 'Z', Zeta)
    CHARMAP('h', eta, 'H', Eta)
    CHARMAP('t', theta, 'T', Theta)
    CHARMAP('i', iota, 'I', Iota)
    CHARMAP('k', kappa, 'K', Kappa)
    CHARMAP('l', lambda, 'L', Lambda)
    CHARMAP('m', mu, 'M', Mu)
    CHARMAP('n', nu, 'N', Nu)
    CHARMAP('x', xi, 'X', Xi)
    CHARMAP('o', omicron, 'O', Omicron)
    CHARMAP('p', pi, 'P', Pi)
    CHARMAP('q', tau, 'Q', Tau)
    CHARMAP('r', rho, 'R', Rho)
    CHARMAP('s', sigma, 'S', Sigma)
    CHARMAP('u', upsilon, 'U', Upsilon)
    CHARMAP('v', phi, 'V', Phi)
    CHARMAP('c', chi, 'C', Chi)
    CHARMAP('f', psi, 'F', Psi)
    CHARMAP('w', omega, 'W', Omega)
    ;
  
  if (src [i] != '%' || (i+1) >= src.length())
    return false;

  char c = src [i+1];
  if (c == '%') /* %% is the symbol for 'percent sign' */
    return false;

  for (std::vector<GreekLetterMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const GreekLetterMap &mi = *it;
    // Look for the first character
    if (c == mi.symbol) {
      LOG_TRACE << "* added greek letter for (" << mi.symbol << ")";
      target.push_back (boost::make_shared<MDE_GreekLetter>(mi.character));
      i += 2;

      return true;
    }
  }

  MSG_WARNING(MDM_UNKNOWN_GREEK, boost::str(boost::format("'%%%c' does not represent a greek letter") % c));
  return false;
}

/**
 * Attempts to interpret a variety of random symbols.

 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
struct SymbolMap { 
  SymbolMap (const std::string &s, const MDE_Symbol::Symbol sym) : search(s), symbol(sym) {}
  std::string search;
  MDE_Symbol::Symbol symbol;
};

bool MathDocument::interpretSymbol (MDEVector &target,
				    const std::string &src, 
				    size_t &i)
{
  #define MAX_SYMBOL_LEN 2
  // Be sure to update MAX_SYMBOL_LEN if longer symbols get added to
  // this table.  Place longer symbols at the top.
  static const std::vector<SymbolMap> map = boost::assign::list_of 
    ( SymbolMap("/\\", MDE_Symbol::THEREFORE) )
    ( SymbolMap("%%", MDE_Symbol::PERCENT) )
    ( SymbolMap("!", MDE_Symbol::FACTORIAL) )
    ;
  
  const std::string temp = src.substr(i, MAX_SYMBOL_LEN);

  for (std::vector<SymbolMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const SymbolMap &mi = *it;
    
    if (strBeginsWith(temp, mi.search)) {
      LOG_TRACE << "* added symbol (" << mi.search << ")";
      target.push_back (boost::make_shared<MDE_Symbol>(mi.symbol));
      i += mi.search.length();
      return true;
    }
  }

  return false;
}

/**
 * Attempts to interpret a fraction: @num~den#

 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty numerator/denominator
 */
bool MathDocument::interpretFraction (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  if (src [i] == '#' || src [i] == '~') {
    MSG_WARNING(MDM_SUSPECT_FRACTION, boost::str(boost::format("found '%c' symbol outside of a fraction") % src [i]));
    return false;
  }

  if (src [i] != '@') 
    return false;

  // Count forward to find the "end fraction" indicator (#). Allow 
  // nested fractions but do not evaluate them as we go for now -- 
  // just try to get the outermost fraction that we see.
  int num_nested_fractions = 0;
  bool foundTerminator = false;
  bool foundDividingLine = false;
  size_t pos;
  std::string numerator;
  std::string denominator;
  for (pos = i+1; pos < src.length(); pos++) {
    // Skip escaped characters or "approximately equal to" symbols
    // that appear within a fraction.
    if (src.substr (pos, 2) == "\\#" || src.substr(pos, 2) == "\\@" 
	|| src.substr (pos, 2) == "\\~" || src.substr(pos, 2) == "~=")
      pos += 2;

    // Look for nested fractions
    if (src [pos] == '@')
      num_nested_fractions++;
    else // Look for the "end fraction" indicators
    if (src [pos] == '#') {
      if (!num_nested_fractions) {
	foundTerminator = true;
	break;
      }
      else
	num_nested_fractions--;
    }
    else // Look for the dividing line if we aren't in a nested fraction
    if (!foundDividingLine && !num_nested_fractions && src [pos] == '~') {
      foundDividingLine = true;
      continue;
    }
  
    // Add this character  to the numerator or denominator as requierd
    if (!foundDividingLine)
      numerator += src [pos];
    else
      denominator += src [pos];
  }

  if (!foundTerminator) {
    MSG_ERROR(MDM_FRACTION_NOT_TERMINATED, boost::str(boost::format("end of line encountered while still inside %d fraction%s") % (num_nested_fractions+1) % (num_nested_fractions >= 2 ? "s" : "")));

    BOOST_THROW_EXCEPTION (MathDocumentParseException());
  }

  // advance cursor
  i = pos + 1;

  LOG_TRACE << "* found fraction: " << numerator << " // " << denominator;

  MDEVector numvec;
  MDEVector denvec;

  numvec = interpretBuffer (numerator);
  denvec = interpretBuffer (denominator);
  
  target.push_back (boost::make_shared<MDE_Fraction>(numvec, denvec));
  return true;
}

/**
 * Attempts to interpret an exponent: ^item or ^(item)
 *
 * An "item" is everything up to the next space, sign of operation,
 * sign of comparison, etc.
 *
 * If ^ is followed by parenthesees then the entire paranthesees is 
 * taken as the exponent.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty exponents: x^() for example
 */
bool MathDocument::interpretExponent (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  if (src [i] != '^') 
    return false;

  std::string exponent_contents;


  // If this is followed by an open paren '(', read until the closing 
  // paren
  i++;
  if (src [i] == '(') {
    int num_nested_parens = 0;
    bool foundTerminator = false;
    size_t pos;
    for (pos = i+1; pos < src.length(); pos++) {
      // Skip escaped characters
      if (src.substr (pos, 2) == "\\(" || src.substr(pos, 2) == "\\)")
	pos += 2;

      // Look for nested parentheses
      if (src [pos] == '(')
	num_nested_parens++;
      else // Look for the closing parens
	if (src [pos] == ')') {
	  if (!num_nested_parens) {
	    foundTerminator = true;
	    break;
	  }
	  else
	    num_nested_parens--;
	}
  
      // Add this character to the contents of the exponent 
      exponent_contents += src [pos];
    }

    if (!foundTerminator) {
      MSG_ERROR(MDM_EXPONENT_NOT_TERMINATED, boost::str(boost::format("text in exponent: '%s'") % exponent_contents));
      BOOST_THROW_EXCEPTION (MathDocumentParseException());
    }

    // advance cursor
    i = pos + 1;
  } else  {
    std::string exponent_terminators = "+/*=<>()[]{} ~@#_";
    size_t pos;

    // Copy characters until we encounter any of the above-mentioned 
    // terminators.  
    //
    // The semi-colon is specifically designated as a terminator, 
    // and should be skipped if it arises.
    //
    // Special cases:
    // - fractions (exponent can begin with a fraction in which case we 
    //   take the whole fraction)
    // - negative numbers (exponent can begin with a minus sign '-' but 
    //   only in the first character)
    if (src [i] == '@') {
      LOG_TRACE << "- found a fractional exponent: handing over to fractions";
      MDEVector fracvec;
      assert (interpretFraction (fracvec, src, i));
      target.push_back (boost::make_shared<MDE_Exponent>(fracvec));
      return true;
    }

    for (pos = i; pos < src.length(); pos++) {
      // skip escaped characters
      if (src [pos] == '\\') {
	pos++;
	continue;
      }

      // On exponent terminators, skip -- the semi-colon should not be part 
      // of the final output.
      if (src [pos] == ';') {
	pos++;
	break;
      }

      // On other terminators, do not "lose them" -- they should wind 
      // up in the final output.
      if (isOneOf (src [pos], exponent_terminators))
	break;

      if (pos == i) 
	exponent_terminators += '-';

      exponent_contents += src [pos];
    }

    i = pos;
  }

  LOG_TRACE << "* found exponent: " << exponent_contents;

  MDEVector v;
  v = interpretBuffer (exponent_contents);
  target.push_back (boost::make_shared<MDE_Exponent>(v));
  return true;
}

/**
 * Attempts to interpret a subscript: _item or _(item)
 *
 * An "item" is everything up to the next space, sign of operation,
 * sign of comparison, etc.
 *
 * If _ is followed by parenthesees then the entire paranthesees is 
 * taken as the subscript.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * ##TODO: warn about empty subscriptions: x_() for example
 */
bool MathDocument::interpretSubscript (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  if (src [i] != '_') 
    return false;

  std::string subscript_contents;


  // If this is followed by an open paren '(', read until the closing 
  // paren
  i++;
  if (src [i] == '(') {
    int num_nested_parens = 0;
    bool foundTerminator = false;
    size_t pos;
    for (pos = i+1; pos < src.length(); pos++) {
      // Skip escaped characters
      if (src.substr (pos, 2) == "\\(" || src.substr(pos, 2) == "\\)")
	pos += 2;

      // Look for nested parentheses
      if (src [pos] == '(')
	num_nested_parens++;
      else // Look for the closing parens
	if (src [pos] == ')') {
	  if (!num_nested_parens) {
	    foundTerminator = true;
	    break;
	  }
	  else
	    num_nested_parens--;
	}
  
      // Add this character to the contents of the exponent 
      subscript_contents += src [pos];
    }

    if (!foundTerminator) {
      MSG_ERROR(MDM_SUBSCRIPT_NOT_TERMINATED, boost::str(boost::format("text in subscript: '%s'") % subscript_contents));
      BOOST_THROW_EXCEPTION (MathDocumentParseException());
    }

    // advance cursor
    i = pos + 1;
  } else  {
    std::string subscript_terminators = "+/*=<>()[]{} ~@#_";
    size_t pos;

    // Copy characters until we encounter any of the above-mentioned 
    // terminators.  
    //
    // The semi-colon is specifically designated as a terminator, 
    // and should be skipped if it arises.
    //
    // Special cases:
    // - fractions (subscript can begin with a fraction in which case we 
    //   take the whole fraction)
    // - negative numbers (subsscript can begin with a minus sign '-' but 
    //   only in the first character)
    if (src [i] == '@') {
      LOG_TRACE << "- found a fractional subscript: handing over to fractions";
      MDEVector fracvec;
      assert (interpretFraction (fracvec, src, i));
      target.push_back (boost::make_shared<MDE_Exponent>(fracvec));
      return true;
    }

    for (pos = i; pos < src.length(); pos++) {
      // skip escaped characters
      if (src [pos] == '\\') {
	pos++;
	continue;
      }

      // On subscript terminators, skip -- the semi-colon should not be part 
      // of the final output.
      if (src [pos] == ';') {
	pos++;
	break;
      }

      // On other terminators, do not "lose them" -- they should wind 
      // up in the final output.
      if (isOneOf (src [pos], subscript_terminators))
	break;

      // If this is the first character considered, add '-' to the list of 
      // possible terminators going forward
      if (pos == i) 
	subscript_terminators += '-';

      subscript_contents += src [pos];
    }

    i = pos;
  }

  LOG_TRACE << "* found subscript: " << subscript_contents;

  MDEVector v;
  v = interpretBuffer (subscript_contents);
  target.push_back (boost::make_shared<MDE_Subscript>(v));
  return true;
}


/**
 * Attempts to interpret a root: _/100 (square root of 100)
 *                            or _/(n+1) (square root of 'n+1')
 *                            or _/3(16) (cube root of 16)
 *                            or _/[n^2](x + 2y + 3c^2) (complex root index)
 *
 * For the basic form, the root is everything up to the next space, sign of 
 * operation, sign of comparison, etc.
 *
 * If _/ is followed by parenthesees then the entire paranthesees is 
 * taken as the root argument.
 *
 * If _/ is followed by a number, and immediately followed by an expression 
 * in paranthesees, then the number is considered the root index and the 
 * entire paranthesees is taken as the root argment.
 *
 * If _/ is followed by a square bracket, then the entire square bracket is 
 * taken as the root index, and the entire paranthesees following that is 
 * taken as the root argument.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
bool MathDocument::interpretRoot (MDEVector &target,
				  const std::string &src, 
				  size_t &i)
{
  if (src.substr(i, 2) != "_/")
    return false;

  std::string root_index;
  std::string root_argument;
  MDEVector root_index_vector;

  i += 2;

  // Look for root indexes, which may be a simple letter or number, or a 
  // more complex expression bounded by square brackets.
  boost::regex root_index_regex("(\\w)+\\(");
  boost::smatch mr;
  if (boost::regex_search(src.substr(i), mr, root_index_regex)) {
    root_index = mr[1];
    i += root_index.length();
  } else if (src [i] == '[') {
    // read until the closing square bracket
    int num_nested_brackets = 0;
    bool foundTerminator = false;
    size_t pos;
    for (pos = i+1; pos < src.length(); pos++) {
      // Skip escaped characters
      if (src.substr (pos, 2) == "\\[" || src.substr(pos, 2) == "\\]")
	pos += 2;

      // Look for nested brackets
      if (src [pos] == '[')
	num_nested_brackets++;
      else // Look for the closing brackets
	if (src [pos] == ']') {
	  if (!num_nested_brackets) {
	    foundTerminator = true;
	    break;
	  }
	  else
	    num_nested_brackets--;
	}
  
      // Add this character to the contents of the root index
      root_index += src [pos];
    }

    if (!foundTerminator) {
      MSG_ERROR(MDM_ROOT_INDEX_NOT_TERMINATED, boost::str(boost::format("text in root index: '%s'") % root_index));
      BOOST_THROW_EXCEPTION (MathDocumentParseException());
    }

    // advance cursor
    i = pos + 1;
  }

  if (!root_index.empty()) {
    LOG_TRACE << "- found a root with an index: generating index vector for '" << root_index << "'";
    root_index_vector = interpretBuffer(root_index);
  }
  else
    LOG_TRACE << "- found a root with no index";

  // If this is followed by an open paren '(', read until the closing 
  // paren; otherwise, read until the end of the item
  if (src [i] == '(') {
    int num_nested_parens = 0;
    bool foundTerminator = false;
    size_t pos;
    for (pos = i+1; pos < src.length(); pos++) {
      // Skip escaped characters
      if (src.substr (pos, 2) == "\\(" || src.substr(pos, 2) == "\\)")
	pos += 2;

      // Look for nested parentheses
      if (src [pos] == '(')
	num_nested_parens++;
      else // Look for the closing parens
	if (src [pos] == ')') {
	  if (!num_nested_parens) {
	    foundTerminator = true;
	    break;
	  }
	  else
	    num_nested_parens--;
	}
  
      // Add this character to the contents of the root
      root_argument += src [pos];
    }

    if (!foundTerminator) {
      MSG_ERROR(MDM_ROOT_NOT_TERMINATED, boost::str(boost::format("text in root: '%s'") % root_argument));
      BOOST_THROW_EXCEPTION (MathDocumentParseException());
    }

    // advance cursor
    i = pos + 1;
  } else  {
    std::string root_terminators = "+/*=<>()[]{} ~@#_";
    size_t pos;

    // Copy characters until we encounter any of the above-mentioned 
    // terminators.  
    //
    // The semi-colon is specifically designated as a terminator, 
    // and should be skipped if it arises.
    //
    // Special cases:
    // - fractions (root can begin with a fraction in which case we 
    //   take the whole fraction)
    if (src [i] == '@') {
      LOG_TRACE << "- for the fractional root, generating root argument via fraction handling";
      MDEVector fracvec;
      assert (interpretFraction (fracvec, src, i));

      target.push_back (boost::make_shared<MDE_Root>(root_index_vector, fracvec));
      return true;
    }

    for (pos = i; pos < src.length(); pos++) {
      // skip escaped characters
      if (src [pos] == '\\') {
	pos++;
	continue;
      }

      // On terminators, skip -- the semi-colon should not be part 
      // of the final output.
      if (src [pos] == ';') {
	pos++;
	break;
      }

      // On other terminators, do not "lose them" -- they should wind 
      // up in the final output.
      if (isOneOf (src [pos], root_terminators))
	break;

      root_argument += src [pos];
    }

    i = pos;
  }

  LOG_TRACE << "* found root: index='" << root_index << "', argument: '" << root_argument << "'";

  MDEVector root_argument_vector = interpretBuffer (root_argument);
  target.push_back (boost::make_shared<MDE_Root>(root_index_vector, root_argument_vector));
  return true;
}

/**
 * Creates a 'generic text/math' block from the characters in buffer
 */
MathDocumentElementPtr MathDocument::makeGeneric (const std::string &buffer)
{
  MathDocumentElementPtr e;

  LOG_TRACE << "creating generic block(" << buffer << "), TextMode=" << inTextMode;

  if (inTextMode) {
    sniffTextForMath (buffer);
    e = boost::make_shared<MDE_TextBlock>(buffer);
  }
  else
    e = boost::make_shared<MDE_MathBlock>(buffer);

  return e;
}

/**
 * Evaluates a block of text to determine whether it appears as though it 
 * might contain mathematical material.  If it does, a warning is added to
 * the processing log.
 */
#define CONTAINS(c) (buffer.find(c) != std::string::npos)
void MathDocument::sniffTextForMath (const std::string &buffer)
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
    MSG_WARNING(MDM_SUSPECT_MATH_IN_TEXT, boost::str(boost::format("found %s in '%s'") % ba::join(suspicious_items, ", ") % buffer));
  }
}
#undef CONTAINS

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

/**
 * @file LaTeXRenderer.cpp
 * LaTeX rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "liblouis-mt.h"
#include "logging.h"
#include "UEBRenderer.h"
#include "MathExceptions.h"

namespace ba = boost::assign;

UEBRenderer::UEBRenderer() : MathRenderer()
{
  internalRenderCount = 0;
  
  status.isNumericMode = false;
  status.isStart = true;
  status.isUsingSpacedOperators = false;
}

UEBRenderer::~UEBRenderer()
{
  lou_free();
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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

  return output;
}

// Returns true if the provided vector consists of a single 'item', as that
// is defined by the Technical Guidelines, s. 7.1 or 12.1
// (An 'item' is: a number; a fraction; a radical; a narrow; a shape; 
//  anything in paranthesees, square brackets, or braces; anything in
//  braille grouping indicators; or the previous character)
bool UEBRenderer::isBrailleItem (const MDEVector &v) const
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

  // Case 2: Fractions, roots, and operators are always items in and of
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

  output += s;
  status.isStart = false;
  
  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

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

  int inlen = extParseChars(s.c_str(), input_buffer.get());
  int outlen = LIBLOUIS_MAXSTRING;

  LOG_TRACE << "Sending " << inlen << " chars to louis using table '" << LIBLOUIS_UEB_G1_TABLE << "': {" << showString(input_buffer.get(), inlen) << "}, max output size=" << outlen;
  if (!lou_translateString(LIBLOUIS_UEB_G1_TABLE,
			   input_buffer.get(), 
			   &inlen,
			   braille_buffer.get(),
			   &outlen,
			   NULL /* no typeform checking */,
			   NULL /* don't care about spacing info */,
			   0 /* no modes set */)) {
    LOG_ERROR << "! Braille translation error on '" << s << "'!";
    std::ostringstream os;
    os << "Error occurred while trying to render braille for text: '" << s << "'";
    BOOST_THROW_EXCEPTION (MathRenderException() <<
			   mdx_error_info(os.str()));
  }

  // showString returns the braille encapsulated in single quotation marks 
  // ('), so we have to trim those out before returning
  LOG_TRACE << "Louis returned " << outlen << " chars: {" << showString(braille_buffer.get(), outlen) << "}";
  braille_string = std::string(showString(braille_buffer.get(), outlen));
  output += braille_string.substr(1, braille_string.length()-2);

  status.isNumericMode = false;
  status.isStart = false;

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

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
  if (boost::iequals(e->getName(), "ExtraOperatorSpacing")) {
    status.isUsingSpacedOperators = (e->getParameters() == "true");
    LOG_TRACE << "- extra operator spacing: " << status.isUsingSpacedOperators;
  }

  return output;
}

std::string UEBRenderer::renderMathModeMarker (const MDE_MathModeMarker *e)
{
  std::string output;

  return output;
}

std::string UEBRenderer::renderTextModeMarker (const MDE_TextModeMarker *e)
{
  std::string output;

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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output = renderTextContent(e->getText() + " ");

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderNumber (const MDE_Number *e)
{
  std::string output;
  std::string printNumber;
  std::string brailleNumber;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  size_t pos = 0;
  printNumber = e->getStandardNotation();
  if (printNumber[pos] == '-') {
    brailleNumber = "-";
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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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
    output = " " + opmap[e->getOperator()] + " ";
  else
    output = opmap [e->getOperator()];

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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

  output = compmap [e->getComparator()];

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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

  output = renderMathContent(charmap [e->getValue()]);
  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

  return output;
}

std::string UEBRenderer::renderSymbol (const MDE_Symbol *e)
{
  std::string output;

  static std::map<MDE_Symbol::Symbol,std::string> symmap = ba::map_list_of
    ( MDE_Symbol::COMMA, UEB_COMMA " ")
    ( MDE_Symbol::FACTORIAL, UEB_FACTORIAL )
    ( MDE_Symbol::LEFT_BRACE, UEB_LEFT_BRACE )
    ( MDE_Symbol::LEFT_BRACKET, UEB_LEFT_BRACKET )
    ( MDE_Symbol::LEFT_PAREN, UEB_LEFT_PAREN )
    ( MDE_Symbol::PERCENT, UEB_PERCENT )
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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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
  if (!isBrailleItem(e->getArgument()))
    output = UEB_GROUP_BEGIN + renderedArgument + UEB_GROUP_END;
  else
    output = renderedArgument;

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
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderRoot (const MDE_Root *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  if (e->getIndex().empty()) { /* simple square root */ 
    std::string renderedRootArgument;

    beginInternalRender();
    status.isNumericMode = false;
    renderedRootArgument = renderVector(e->getArgument());
    endInternalRender();

    LOG_TRACE << "- rendered root: " << renderedRootArgument;

    output = renderMathContent(UEB_ROOT_BEGIN + renderedRootArgument + UEB_ROOT_END);
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
    
    output = renderMathContent(UEB_ROOT_BEGIN + renderedRootIndex + renderedRootArgument + UEB_ROOT_END);
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderFraction (const MDE_Fraction *e)
{
  std::string renderedNumerator, renderedDenominator;
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  beginInternalRender();
  status.isNumericMode = false;
  renderedNumerator = renderVector (e->getNumerator());

  status.isNumericMode = false;
  renderedDenominator = renderVector (e->getDenominator());
  endInternalRender();

  // Distinguish between 'simple' fractions (numeric only) and more complex
  // fractions to decide how to render. If the numerator and denominator 
  // both appear to start with numbers, check whether they contain ONLY 
  // numbers (after the number sign).
  bool isSimpleFraction =
    (renderedNumerator.substr(0, 1) == UEB_NUMBER_SIGN &&
     renderedDenominator.substr(0, 1) == UEB_NUMBER_SIGN &&
     containsOnly(renderedNumerator.substr(1), UEB_NUMERIC_MODE_SYMBOLS) &&
     containsOnly(renderedDenominator.substr(1), UEB_NUMERIC_MODE_SYMBOLS));

  LOG_TRACE << "- rendered numerator:   " << renderedNumerator;
  LOG_TRACE << "- rendered denominator: " << renderedDenominator;
  LOG_TRACE << "- is simple fraction? " << isSimpleFraction;

  if (isSimpleFraction)
    output = renderMathContent(boost::str(boost::format("%s" UEB_SIMPLE_FRAC_DIVIDER "%s") % renderedNumerator % renderedDenominator));
  else
    output = renderMathContent(boost::str(boost::format(UEB_FRAC_BEGIN "%s" UEB_FRAC_DIVIDER "%s" UEB_FRAC_END) % renderedNumerator % renderedDenominator));

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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
  endInternalRender();

  // Insert grouping symbols only if the exponent contents is not an 'item'
  // (See isBrailleItem() for details)
  if (isBrailleItem(e->getValue())) {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_UP "%s") % renderedExponent));
  } else {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_UP UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedExponent));
  }
  
  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
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
  endInternalRender();

  // Insert grouping symbols only if the subscript contents is not an 'item'
  // (See isBrailleItem() for details)
  if (isBrailleItem(e->getValue())) {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN "%s") % renderedSubscript));
  } else {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedSubscript));
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}


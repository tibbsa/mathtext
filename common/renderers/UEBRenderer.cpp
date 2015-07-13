/**
 * @file LaTeXRenderer.cpp
 * LaTeX rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

#include <map>
#include <sstream>

#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/scoped_array.hpp>

#include "logging.h"
#include "liblouis-mt.h"
#include "MathDocument.h"
#include "MathExceptions.h"
#include "UEBRenderer.h"


namespace ba = boost::assign;

UEBRenderer::UEBRenderer (const MathDocument &md) : MathRenderer(md)
{
  internalRenderCount = 0;
  
  status.isNumericMode = false;
  status.isStart = true;
}

UEBRenderer::~UEBRenderer()
{
  lou_free();
}

void UEBRenderer::beginInternalRender (void)
{
  internalRenderCount++;
  statusStack.push (status);

  LOG_TRACE << "## Internal render begun, nesting level " << internalRenderCount << ", status: #=" << status.isNumericMode << ", S=" << status.isStart;
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
  LOG_TRACE << "## Internal render end, nesting level " << internalRenderCount << ", status: #=" << status.isNumericMode << ", S=" << status.isStart;
}

std::string UEBRenderer::translateToBraille (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  output = translateBrailleNumbers (s);
  output = translateBraillePunctuation (output);

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";

  return output;
}

std::string UEBRenderer::translateBrailleNumbers (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  // If we are already in numeric mode, and the next character is not
  // a number but could be confused as such, add a grade 1 terminator.
  for (size_t pos = 0; pos < s.length(); pos++) {
    char c = s [pos];
    char c2 = (pos < (s.length() - 1)) ? s [pos+1] : 0;
    LOG_TRACE << "@" << pos << ": c=" << c << ", c2=" << c2 << ", INM=" << status.isNumericMode;

    if (status.isNumericMode && !isdigit(c) && c != '.') {
      // If a number is followed by letters A-J, insert a grade 1 (aka 
      // 'letter' indicator) to return to alphabet mode
      if ((c >= 'a' && c <= 'j') || (c >= 'A' && c <= 'J')) {
	LOG_TRACE << " -- inserting grade 1 indicator for following letter";
	output += UEB_G1;
      }

      output += c;
      status.isNumericMode = false;
      continue;
    }

    if (!status.isNumericMode) {
      // Insert a number sign before the first digit, or before the decimal 
      // point if we get a number like ".74". 
      if (isdigit(c) || (c == '.' && isdigit(c2))) {
	LOG_TRACE << " -- adding number sign and entering numeric mode";
	output += UEB_NUMBER_SIGN;
	status.isNumericMode = true;
      }
      else {
	output += c;
	continue;   
      }   
    }

    // In braille, numbers 1-9(0) are represented by letters A-J.  On the 
    // ASCII tables, numbers 1-9 are code 49-57, and number 0 is code 48.
    // The beginning of the alphabet (uppercase) is at #65, so 1=A=65, etc.
    // In other words, adding 16 to the digit will get the uppercase letter.

    // Pass decmials through as-is for now
    if (c == '.') {
      LOG_TRACE << " -- passing through decimal";
      output += ".";
    } else if (c == '0') { // handle 0
      LOG_TRACE << " -- handling '0' digit";
      output += "J";
    } else if (isdigit(c)) { // handle 1-9
      LOG_TRACE << " -- handling '1-9' digit";
      output += boost::str(boost::format("%c") % (char)(16 + (int)c));
    } else {
      // Should never get here.
      assert(0);
    }
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

// called to output what is known to be 'math' material. takes care of 
// any necessary mode switches, etc., 
std::string UEBRenderer::renderMathContent (const std::string &s)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << s << ")";
  logIncreaseIndent();

  output = s;
  if (!doingInternalRender())
    isStartOfLine = false;
  
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

  boost::scoped_array<ll_widechar> input_buffer (new ll_widechar [BRAILLE_TRANSLATION_BUFFER_SIZE+1]);
  boost::scoped_array<ll_widechar> braille_buffer (new ll_widechar [BRAILLE_TRANSLATION_BUFFER_SIZE+1]);

  int inlen = extParseChars(s.c_str(), input_buffer.get());
  int outlen;
  
  if (!lou_translateString(BRAILLE_TRANSLATION_TABLE,
			   input_buffer.get(), 
			   &inlen,
			   braille_buffer.get(),
			   &outlen,
			   NULL /* no typeform checking */,
			   NULL /* don't care about spacing info */,
			   1 /* noContractions */)) {
    LOG_ERROR << "! Braille translation error on '" << s << "'!";
    std::ostringstream os;
    os << "Error occurred while trying to render braille for text: '" << s << "'";
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			   mdx_error_info(os.str()));
  }

  // showString returns the braille encapsulated in single quotation marks 
  // ('), so we have to trim those out before returning
  braille_string = std::string(showString(braille_buffer.get(), outlen));
  output += braille_string.substr(1, braille_string.length()-2);

  status.isNumericMode = false;

  if (!doingInternalRender())
    isStartOfLine = false;

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

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output = renderMathContent(translateToBraille(e->getText()));
  
  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  std::string output;

  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output = renderMathContent(translateToBraille(e->getText() + " "));

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderNumber (const MDE_Number *e)
{
  std::string output;
  LOG_TRACE << ">> " << __func__ << ": (" << *e << ")";
  logIncreaseIndent();

  output = renderMathContent(translateToBraille(e->getStandardNotation()));

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
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			   mdx_error_info(os.str()));
  }

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
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
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
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
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
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			   mdx_error_info(os.str()));
  }

  output = renderMathContent(symmap [e->getSymbol()]);

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
    renderedRootArgument = renderFromVector(e->getArgument());
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
    renderedRootArgument = renderFromVector(e->getArgument());
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
  renderedNumerator = renderFromVector (e->getNumerator());

  status.isNumericMode = false;
  renderedDenominator = renderFromVector (e->getDenominator());
  endInternalRender();

  LOG_TRACE << "- rendered numerator:   " << renderedNumerator;
  LOG_TRACE << "- rendered denominator: " << renderedDenominator;

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
  renderedExponent = renderFromVector (e->getValue());
  endInternalRender();

  // If the exponent is only a number, do not use grouping symbols
  //## TODO: There are other things that count as a 'single item' for this 
  //         purpose, such as fractions.  Optimize for other items?
  if (renderedExponent.substr(0,1) == UEB_NUMBER_SIGN && 
      containsOnly(renderedExponent.substr(1), UEB_NUMERIC_MODE_SYMBOLS)) {
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
  renderedSubscript = renderFromVector (e->getValue());
  endInternalRender();

  // If the subscript is only a number, do not use grouping symbols
  //## TODO: There are other things that count as a 'single item' for this 
  //         purpose, such as fractions.  Optimize for other items?
  if (renderedSubscript.substr(0, 1) == UEB_NUMBER_SIGN && 
      containsOnly(renderedSubscript.substr(1), UEB_NUMERIC_MODE_SYMBOLS)) {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN "%s") % renderedSubscript));
  } else {
    output = renderMathContent(boost::str(boost::format(UEB_LEVEL_DOWN UEB_GROUP_BEGIN "%s" UEB_GROUP_END) % renderedSubscript));
  }

  logDecreaseIndent();
  LOG_TRACE << "<< " << __func__ << ": (" << output << ")";
  return output;
}

std::string UEBRenderer::renderUnsupported (const MathDocumentElement *e)
{
  return boost::str(boost::format("<? %s ?>") % typeid(*e).name());
}
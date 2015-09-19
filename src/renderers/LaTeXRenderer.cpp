/**
* @file
* @copyright Copyright (C) 2001-2015 Anthony Tibbs <anthony@tibbs.ca>
* \par License
* This project is released under the GNU Public License, version 3.
* See COPYING or http://www.gnu.org/licenses/ for more information.
*/

#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>

#include "logging.h"

#include "MathExceptions.h"
#include "LaTeXRenderer.h"

namespace ba = boost::assign;

LaTeXRenderer::LaTeXRenderer() : MathRenderer()
{
  currentBlockType = MATH;
  currentSegmentType = MATH;

  writerLineMode = UNKNOWN;
  writerCurrentMode = UNKNOWN;

  isStartOfLine = true;
  isBracketSizingEnabled = true;

  internalRenderCount = 0;
}

/**
 * @copydoc MathRenderer::renderDocument
 *
 * The rendered document includes the LaTeX preamble, \\begin, \\end, etc.
 */
std::string LaTeXRenderer::renderDocument (const MathDocument &document)
{
  std::string output;

  output += "\\documentclass[12pt]{article}\n";
  output += "\\usepackage{amssymb}\n";
  output += "\\usepackage[fleqn]{amsmath}\n";
  output += "\\usepackage{amstext}\n";
  output += "\\usepackage{eurosym}\n";
  output += "\\usepackage{textcomp}\n";
  output += "\\usepackage{wasysym}\n";
  output += "\\usepackage[margin=1in]{geometry}\n";
  output += "\\usepackage{fancyhdr}\n";
  output += "\\usepackage{lastpage}\n";
  output += "\\pagestyle{fancy} % Set default page style to fancy\n";
  output += "\\renewcommand{\\headrulewidth}{0pt} % Remove header rule\n";
  output += "\\fancyhead{} % Remove all header contents\n";
  output += "\\cfoot{Page \\thepage\\ of \\pageref{LastPage}} % Page X of Y in the footer (centered)\n";
  output += "\\parskip 0in \\parindent 0in\n";
  output += "\\begin{document}\n\n";

  output += MathRenderer::renderDocument (document);

  output += "\n\\end{document}\n";
  return output;
}

void LaTeXRenderer::getInterpreterCommandList (std::vector<std::string> &cmdlist)
{
  cmdlist.push_back("NoBracketSizing");
}

std::string LaTeXRenderer::makeLaTeXSafe (const std::string &input)
{
  std::string output;
  bool bSawLatexEscape = false;

  for (size_t pos = 0; pos < input.length(); pos++) {
    char ch = input [pos];

    if (!bSawLatexEscape) {
      if (ch == '#') {
	output += "\\#";
      } else if (ch == '\\') {
	output += "\\backslash ";
      } else if (ch == '_') {
	output += "\\underline ";
      } else if (ch == '^') {
	output += "\\^{}";
      } else if (ch == '{') {
	output += "\\lbrace ";
      } else if (ch == '}') {
	output += "\\rbrace ";
      }
      else if (ch == '\\') {
	bSawLatexEscape = true;
	output += "\\";
      } else {
	output += ch;
      }
    }
    else {
      output += ch;
      bSawLatexEscape = false;
    }
  }

  return output;
}

std::string LaTeXRenderer::status (void) const
{
  std::string output;

  output += "blk:";
  switch (currentBlockType) {
  case MATH: output += "M"; break;
  case TEXT: output += "T"; break;
  case UNKNOWN: output += "?"; break;
  }

  output += ", seg:";
  switch (currentSegmentType) {
  case MATH: output += "M"; break;
  case TEXT: output += "T"; break;
  case UNKNOWN: output += "?"; break;
  }

  output += ", wl:";
  switch (writerLineMode) {
  case MATH: output += "M"; break;
  case TEXT: output += "T"; break;
  case UNKNOWN: output += "?"; break;
  }

  output += ", ws:";
  switch (writerCurrentMode) {
  case MATH: output += "M"; break;
  case TEXT: output += "T"; break;
  case UNKNOWN: output += "?"; break;
  }

  if (isStartOfLine)
    output += " *SOL*";

  if (doingInternalRender())
    output += boost::str(boost::format(" *MODE SWITCH INHIBITED - irc:%u*") % internalRenderCount);

  return output;
}

void LaTeXRenderer::beginInternalRender (void)
{
  internalRenderCount++;
}

bool LaTeXRenderer::doingInternalRender (void) const
{
  return (internalRenderCount > 0);
}

void LaTeXRenderer::endInternalRender (void)
{
  assert (internalRenderCount != 0);
  internalRenderCount--;
}

// called to output what is known to be 'math' material. takes care of
// any necessary LaTeX mode switches, etc.,
std::string LaTeXRenderer::renderMathContent (const std::string &s)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderMathContent (" << s << ")";
  LOG_TRACE << "   >> " << status();
  logIncreaseIndent();

  /*
  ** Enter or switch to math mode as required.  (If we're doing an internal
  ** render, e.g. the numerator of a fraction or some such, don't do this.)
  */
  if (!doingInternalRender()) {
    if (writerCurrentMode != MATH) {
      LOG_TRACE << "* LaTeX mode switch needed: currently in " << (writerCurrentMode == TEXT ? "text" : "unknown") << " mode";
      if (writerCurrentMode == UNKNOWN) {
	// We should only get here if we're at the start of a line and
	// we haven't yet made up our mind as to the line mode.
	assert (writerLineMode == UNKNOWN);
	assert (isStartOfLine == true);

	LOG_TRACE << "* LaTeX line will be in math mode";
	writerLineMode = MATH;
	writerCurrentMode = MATH;

	output += "\\[ ";
      } else { /* writerCurrentMode == TEXT */
	assert (writerLineMode != UNKNOWN);

	// If this is a math line, end the \text{...} segment. If it is a text
	// line, begin an inline math segment ($...$).
	if (writerLineMode == MATH) {
	  LOG_TRACE << "* LaTeX math line, ending text segment";
	  output += "} ";
	} else { /* writerLineMode == TEXT */
	  LOG_TRACE << "* LaTeX text line, beginning math segment";
	  output += " $";
	}

	writerCurrentMode = MATH;
      }

      LOG_TRACE << "   >> " << status();
    }
  }

  output += s;
  if (!doingInternalRender())
    isStartOfLine = false;

  logDecreaseIndent();
  LOG_TRACE << "exit LaTeXRenderer::renderMathContent: " << output;
  LOG_TRACE << "  >> " << status();

  return output;
}

std::string LaTeXRenderer::renderTextContent (const std::string &s)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderTextContent (" << s << ")";
  LOG_TRACE << "   >> " << status();
  logIncreaseIndent();

  /*
  ** Enter or switch to text mode as required.  (If we're doing an internal
  ** render, e.g. the numerator of a fraction or some such, don't do this.)
  */
  if (!doingInternalRender()) {
    if (writerCurrentMode != TEXT) {
      LOG_TRACE << "* LaTeX mode switch needed: currently in " << (writerCurrentMode == MATH ? "math" : "unknown") << " mode";
      if (writerCurrentMode == UNKNOWN) {
	// We should only get here if we're at the start of a line and
	// we haven't yet made up our mind as to the line mode.
	assert (writerLineMode == UNKNOWN);
	assert (isStartOfLine == true);

	LOG_TRACE << "* LaTeX line will be in text mode";
	writerLineMode = TEXT;
	writerCurrentMode = TEXT;

	output += "\\par ";
      } else { /* writerCurrentMode == MATH */
	assert (writerLineMode != UNKNOWN);

	// If this is a math line, begin the \text{...} segment. If it is a text
	// line, end  an inline math segment ($...$).
	if (writerLineMode == MATH) {
	  LOG_TRACE << "* LaTeX math line, beginning text segment";
	  output += "\\text{ ";
	} else { /* writerLineMode == TEXT */
	  LOG_TRACE << "* LaTeX text line, ending math segment";
	  output += "$ ";
	}

	writerCurrentMode = TEXT;
      }
    }
  }

  output += makeLaTeXSafe(s);
  isStartOfLine = false;

  logDecreaseIndent();
  LOG_TRACE << "exit LaTeXRenderer::renderTextContent: " << output;
  LOG_TRACE << "  >> " << status();

  return output;
}

std::string LaTeXRenderer::renderSourceLine (const MDE_SourceLine *e)
{
  std::string output;

  output = "%% " + e->getString() + "\n";
  LOG_TRACE << "%% " << e->getString();

  return output;
}

std::string LaTeXRenderer::renderCommand (const MDE_Command *e)
{
  std::string output;

  output = "%% COMMAND: " + e->getString() + "\n";
  LOG_TRACE << output;

  // automatic sizing of group enclosures (brackets) works well at producing
  // nicely formatted output, but fails if a single line of math breaks
  // across multiple print lines.  This allows that to be disabled.
  if (boost::iequals(e->getName(), "NoBracketSizing")) {
    if (boost::iequals(e->getParameters(), "true")) {
      isBracketSizingEnabled = false;
    } else if (boost::iequals(e->getParameters(), "false")) {
      isBracketSizingEnabled = true;
    } else {
      LOG_ERROR << "! Bracket sizing toggle received invalid argument: " << e->getParameters();
      std::ostringstream os;
      os << "The 'NoBracketSizing' command expects to either be enabled or disabled -- invalid parameter provided: '" << e->getParameters() << "'";
      BOOST_THROW_EXCEPTION (MathRenderException() <<
			     mdx_error_info(os.str()));

    }
  }

  return output;
}

std::string LaTeXRenderer::renderMathModeMarker (const MDE_MathModeMarker *e)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderMathModeMarker (" << *e << ")";
  LOG_TRACE << "  >> " << status();
  logIncreaseIndent();

  if (isStartOfLine)
    currentBlockType = MATH;

  currentSegmentType = MATH;

  logDecreaseIndent();
  LOG_TRACE << "exit LaTeXRenderer::renderMathModeMarker: no output";
  LOG_TRACE << "  >> " << status();

  return output;
}

std::string LaTeXRenderer::renderTextModeMarker (const MDE_TextModeMarker *e)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderTextModeMarker: (" << *e << ")";
  LOG_TRACE << "  >> " << status();
  logIncreaseIndent();

  if (isStartOfLine)
    currentBlockType = TEXT;

  currentSegmentType = TEXT;

  logDecreaseIndent();
  LOG_TRACE << "exit LaTeXRenderer::renderTextModeMarker: no output";
  LOG_TRACE << "  >> " << status();

  return output;
}

std::string LaTeXRenderer::renderLineBreak (const MDE_LineBreak *e)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderLineBreak: (" << *e << ")";
  LOG_TRACE << "  >> " << status();
  logIncreaseIndent();

  if (writerLineMode == MATH) {
    assert (writerCurrentMode != UNKNOWN);

    if (writerCurrentMode == TEXT) {
      LOG_TRACE << "* LaTeX closing text block in math line";
      output += "}";
      writerCurrentMode = MATH;
    }

    LOG_TRACE << "* LaTeX closing math environment on math line";
    output += " \\]";
  } else if (writerLineMode == TEXT) {
    assert (writerCurrentMode != UNKNOWN);

    if (writerCurrentMode == TEXT) {
      LOG_TRACE << "* LaTeX end of line in text mode - nothing to be done";
    } else {
      LOG_TRACE << "* LaTeX end of text line in math mode - ending math segment";
      output += "$";
      writerCurrentMode = TEXT;
    }
  } else {
    // We should only get here if this is a blank line
    assert (isStartOfLine == true);
    output += "\\vspace{10pt}";
  }

  writerLineMode = UNKNOWN;
  writerCurrentMode = UNKNOWN;

  output += "\n";
  isStartOfLine = true;

  logDecreaseIndent();
  LOG_TRACE << "exit LaTeXRenderer::renderLineBreak: " << output;
  LOG_TRACE << "  >> " << status();

  return output;
}

std::string LaTeXRenderer::renderTextBlock (const MDE_TextBlock *e)
{
  std::string text;

  text = e->getText();

  return renderTextContent(text);
}

std::string LaTeXRenderer::renderMathBlock (const MDE_MathBlock *e)
{
  return renderMathContent(e->getText());
}

std::string LaTeXRenderer::renderGroup (const MDE_Group *e)
{
  std::string renderedContents;
  std::string openChar, closeChar;

  beginInternalRender();
  renderedContents = renderVector (e->getContents());
  endInternalRender();

  switch (e->getType()) {
  case MDE_Group::PARENTHESES:
    openChar = "(";
    closeChar = ")";
    break;

  case MDE_Group::BRACKETS:
    openChar = "[";
    closeChar = "]";
    break;

  case MDE_Group::BRACES:
    openChar = "\\{";
    closeChar = "\\}";
    break;

  default:
    assert(0);
  }

  if (isBracketSizingEnabled)
    return renderMathContent("\\left" + openChar + renderedContents + "\\right" + closeChar);
  else
    return renderMathContent(openChar + renderedContents + closeChar);

}

std::string LaTeXRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  std::string qnumber, output;
  qnumber = e->getText();
  output = boost::str(boost::format("\\text{%s}\\thickspace ") % qnumber);
  return renderMathContent(output);
}

std::string LaTeXRenderer::renderNumber (const MDE_Number *e)
{
  return renderMathContent(e->getStandardNotation());
}

std::string LaTeXRenderer::renderOperator (const MDE_Operator *e)
{
  switch (e->getOperator()) {
  case MDE_Operator::ADDITION:
    return renderMathContent(" + ");

  case MDE_Operator::SUBTRACTION:
    return renderMathContent(" - ");

  case MDE_Operator::DIVISION:
    return renderMathContent(" \\div ");

  case MDE_Operator::MULTIPLICATION:
    return renderMathContent(" \\times ");

  default:
    assert(false);
    return renderTextContent (" **OPERATOR ERROR** ");
  }
}

std::string LaTeXRenderer::renderComparator (const MDE_Comparator *e)
{
  switch (e->getComparator()) {
  case MDE_Comparator::LESS_THAN:
    return renderMathContent (" < ");

  case MDE_Comparator::GREATER_THAN:
    return renderMathContent (" > ");

  case MDE_Comparator::EQUALS:
    return renderMathContent (" = ");

  case MDE_Comparator::APPROX_EQUALS:
    return renderMathContent (" \\approx ");

  case MDE_Comparator::NOT_EQUALS:
    return renderMathContent (" \\neq ");

  case MDE_Comparator::GREATER_THAN_EQUALS:
    return renderMathContent (" \\geq ");

  case MDE_Comparator::LESS_THAN_EQUALS:
    return renderMathContent (" \\leq ");

  default:
    assert(false);
    return renderTextContent (" **COMPARATOR ERROR** ");
  }
}

std::string LaTeXRenderer::renderGreekLetter (const MDE_GreekLetter *e)
{
#define MAP(a) (MDE_GreekLetter::a, "\\" #a)
#define MAPTO(a, b) (MDE_GreekLetter::a, b)

  static std::map<MDE_GreekLetter::Character,std::string> charmap = ba::map_list_of
    MAP(alpha) MAPTO(Alpha, "A")
    MAP(beta) MAPTO(Beta, "B")
    MAP(gamma) MAP(Gamma)
    MAP(delta) MAP(Delta)
    MAP(epsilon) MAPTO(Epsilon, "\\varepsilon")
    MAP(zeta) MAPTO(Zeta, "Z")
    MAP(eta) MAPTO(Eta, "H")
    MAP(theta) MAP(Theta)
    MAP(iota) MAPTO(Iota, "I")
    MAP(kappa) MAPTO(Kappa, "K")
    MAP(lambda) MAP(Lambda)
    MAP(mu) MAPTO(Mu, "M")
    MAP(nu) MAPTO(Nu, "N")
    MAP(xi) MAP(Xi)
    MAPTO(omicron, "o") MAPTO(Omicron, "O")
    MAP(pi) MAP(Pi)
    MAP(rho) MAPTO(Rho, "P")
    MAP(sigma) MAP(Sigma)
    MAP(tau) MAPTO(Tau, "T")
    MAP(upsilon) MAP(Upsilon)
    MAP(phi) MAP(Phi)
    MAP(chi) MAPTO(Chi, "X")
    MAP(psi) MAP(Psi)
    MAP(omega) MAP(Omega)
    ;

#undef MAP
#undef MAPTO
  assert(charmap.count(e->getValue()) == 1);
  return renderMathContent(charmap [e->getValue()]);
}

std::string LaTeXRenderer::renderSymbol (const MDE_Symbol *e)
{
  static std::map<MDE_Symbol::Symbol,std::string> map = ba::map_list_of
    ( MDE_Symbol::COMMA, "," )
    ( MDE_Symbol::CURRENCY_CENTS, "\\cent " )
    ( MDE_Symbol::CURRENCY_EURO, "\\euro " )
    ( MDE_Symbol::CURRENCY_FRANC, "F" )
    ( MDE_Symbol::CURRENCY_POUND, "\\pounds " )
    ( MDE_Symbol::CURRENCY_DOLLAR, "\\$" )
    ( MDE_Symbol::CURRENCY_YEN, "Y " )
    ( MDE_Symbol::FACTORIAL, "!" )
    ( MDE_Symbol::LEFT_BRACE, "\\{" )
    ( MDE_Symbol::LEFT_BRACKET, "[" )
    ( MDE_Symbol::LEFT_PAREN, "(" )
    ( MDE_Symbol::PERCENT, "\\%" )
    ( MDE_Symbol::PERIOD, ".")
    ( MDE_Symbol::RIGHT_BRACE, "\\}" )
    ( MDE_Symbol::RIGHT_BRACKET, "]" )
    ( MDE_Symbol::RIGHT_PAREN, ")" )
    ( MDE_Symbol::THEREFORE, "\\therefore " )
    ;

  assert (map.count(e->getSymbol()) == 1);

  return renderMathContent(map[e->getSymbol()]);
}

std::string LaTeXRenderer::renderModifier (const MDE_Modifier *e)
{
  std::string renderedArgument;
  std::string output;

  beginInternalRender();
  renderedArgument = renderVector (e->getArgument());
  endInternalRender();

  switch (e->getModifier()) {
  case MDE_Modifier::OVER_ARROW_RIGHT:
    output = boost::str(boost::format("\\overrightarrow{%s}") % renderedArgument);
    break;

  case MDE_Modifier::OVER_BAR:
    output = boost::str(boost::format("\\overline{%s}") % renderedArgument);
    break;

  case MDE_Modifier::OVER_HAT:
    output = boost::str(boost::format("\\hat{%s}") % renderedArgument);
    break;

  default:
    assert(0);
  }

  return renderMathContent(output);
}

std::string LaTeXRenderer::renderRoot (const MDE_Root *e)
{
  std::string renderedIndex, renderedArgument;

  beginInternalRender();
  renderedIndex = renderVector (e->getIndex());
  renderedArgument = renderVector (e->getArgument());
  endInternalRender();

  if (!renderedIndex.empty()) {
    return renderMathContent(boost::str(boost::format("\\sqrt[%s]{%s}") % renderedIndex % renderedArgument));
  } else {
    return renderMathContent(boost::str(boost::format("\\sqrt{%s}") % renderedArgument));
  }
}

std::string LaTeXRenderer::renderSummation (const MDE_Summation *e)
{
  std::string renderedLowerBound, renderedUpperBound;
  std::string output;

  beginInternalRender();
  renderedLowerBound = renderVector (e->getLowerBound());
  renderedUpperBound = renderVector (e->getUpperBound());
  endInternalRender();

  output = "\\sum";
  if (!renderedLowerBound.empty())
    output += boost::str(boost::format("_{%s}") % renderedLowerBound);

  if (!renderedUpperBound.empty())
    output += boost::str(boost::format("^{%s}") % renderedUpperBound);

  output += " ";
  return renderMathContent(output);
}

std::string LaTeXRenderer::renderFraction (const MDE_Fraction *e)
{
  std::string renderedNumerator, renderedDenominator;

  beginInternalRender();
  renderedNumerator = renderVector (e->getNumerator());
  renderedDenominator = renderVector (e->getDenominator());
  endInternalRender();

  return renderMathContent(boost::str(boost::format("\\frac{%s}{%s}") % renderedNumerator % renderedDenominator));
}

std::string LaTeXRenderer::renderExponent (const MDE_Exponent *e)
{
  std::string renderedExponent;

  beginInternalRender();
  renderedExponent = renderVector (e->getValue());
  endInternalRender();

  return renderMathContent(boost::str(boost::format("^{%s}") % renderedExponent));
}

std::string LaTeXRenderer::renderSubscript (const MDE_Subscript *e)
{
  std::string renderedSubscript;

  beginInternalRender();
  renderedSubscript = renderVector (e->getValue());
  endInternalRender();

  return renderMathContent(boost::str(boost::format("_{%s}") % renderedSubscript));
}

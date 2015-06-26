/**
 * @file LaTeXRenderer.cpp
 * LaTeX rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "logging.h"
#include "MathDocument.h"
#include "MathExceptions.h"
#include "LaTeXRenderer.h"

LaTeXRenderer::LaTeXRenderer (const MathDocument &md) : MathRenderer(md)
{
  currentBlockType = MATH;
  currentSegmentType = MATH;

  writerLineMode = UNKNOWN;
  writerCurrentMode = UNKNOWN;

  isStartOfLine = true;
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

  return output;
}

// called to output what is known to be 'math' material. takes care of 
// any necessary LaTeX mode switches, etc.
std::string LaTeXRenderer::renderMathContent (const std::string &s)
{
  std::string output;

  LOG_TRACE << "enter LaTeXRenderer::renderMathContent (" << s << ")";
  LOG_TRACE << "   >> " << status();
  logIncreaseIndent();

  /*
  ** Enter or switch to math mode as required
  */
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

  output += s;
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
  ** Enter or switch to text mode as required
  */
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

  output += s;
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
    } else {
      LOG_TRACE << "* LaTeX closing math environment on math line";
      output += " \\]";
    }
  } else if (writerLineMode == TEXT) {
    assert (writerCurrentMode != UNKNOWN);

    if (writerCurrentMode == TEXT) {
      LOG_TRACE << "* LaTeX end of line in text mode - nothing to be done";
    } else {
      LOG_TRACE << "* LaTeX end of text line in math mode - ending math segment";
      output += "$";
      writerCurrentMode = TEXT;
    }
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
  std::string blockText = boost::trim_copy(e->getText());
  if (blockText.empty())
    return std::string();

  return renderMathContent(blockText);
}

std::string LaTeXRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  std::string qnumber, output;
  qnumber = e->getText();
  qnumber = boost::str(boost::format("%s \\thickspace ") % qnumber);
  output = renderMathContent(qnumber);
  return output;
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
    return std::string (" \\times ");

  default:
    assert(false);
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
  }
}

std::string LaTeXRenderer::renderGreekLetter (const MDE_GreekLetter *e)
{
  return boost::str(boost::format ("[%s]") % e->getName());
}

std::string LaTeXRenderer::renderFraction (const MDE_Fraction *e)
{
  std::string renderedNumerator, renderedDenominator;

  renderedNumerator = renderFromVector (e->getNumerator());
  renderedDenominator = renderFromVector (e->getDenominator());

  return boost::str(boost::format("[Fraction] (%s) over (%s) [End Fraction]") %  renderedNumerator % renderedDenominator);
}

std::string LaTeXRenderer::renderExponent (const MDE_Exponent *e)
{
  std::string renderedExponent;

  renderedExponent = renderFromVector (e->getValue());

  return boost::str(boost::format("[Exponent] %s [End Exponent]") % renderedExponent);
}

std::string LaTeXRenderer::renderSubscript (const MDE_Subscript *e)
{
  std::string renderedSubscript;

  renderedSubscript = renderFromVector (e->getValue());

  return boost::str(boost::format("[Sub] %s [End Sub]") % renderedSubscript);
}

std::string LaTeXRenderer::renderUnsupported (const MathDocumentElement *e)
{
  return boost::str(boost::format("<? %s ?>") % typeid(*e).name());
}

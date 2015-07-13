/**
 * @file TextRenderer.cpp
 * Plain-text rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "MathDocument.h"
#include "MathExceptions.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer (const MathDocument &md) : MathRenderer(md)
{
}

std::string TextRenderer::renderMathModeMarker (const MDE_MathModeMarker *e)
{
  return std::string(/* no output*/);
}

std::string TextRenderer::renderTextModeMarker (const MDE_TextModeMarker *e)
{
  return std::string(/* no output*/);
}

std::string TextRenderer::renderLineBreak (const MDE_LineBreak *e)
{
  return std::string("\n");
}

std::string TextRenderer::renderTextBlock (const MDE_TextBlock *e)
{
  return e->getText();
}

std::string TextRenderer::renderMathBlock (const MDE_MathBlock *e)
{
  std::string blockText = boost::trim_copy(e->getText());
  if (blockText.empty())
    return std::string(" ");
  else
    return boost::str(boost::format("«%s» ") % blockText);
}

std::string TextRenderer::renderItemNumber (const MDE_ItemNumber *e)
{
  return (e->getText() + " ");
}

std::string TextRenderer::renderOperator (const MDE_Operator *e)
{
  switch (e->getOperator()) {
  case MDE_Operator::ADDITION:
    return std::string (" (plus) ");

  case MDE_Operator::SUBTRACTION:
    return std::string (" (minus) ");

  case MDE_Operator::DIVISION:
    return std::string (" (divided by) ");

  case MDE_Operator::MULTIPLICATION: 
    return std::string (" (times) ");

  default:
    assert(false);
  }
}

std::string TextRenderer::renderComparator (const MDE_Comparator *e)
{
  switch (e->getComparator()) {
  case MDE_Comparator::LESS_THAN:
    return std::string (" (is less than) ");

  case MDE_Comparator::GREATER_THAN:
    return std::string (" (is greater than) ");

  case MDE_Comparator::EQUALS:
    return std::string (" (is equal to) ");

  case MDE_Comparator::APPROX_EQUALS:
    return std::string (" (approximately equals) ");

  case MDE_Comparator::NOT_EQUALS:
    return std::string (" (is not equal to) ");

  case MDE_Comparator::GREATER_THAN_EQUALS:
    return std::string (" (is greater than or equal to) ");

  case MDE_Comparator::LESS_THAN_EQUALS:
    return std::string (" (is less than or equal to) ");

  default:
    assert(false);
  }
}

std::string TextRenderer::renderGreekLetter (const MDE_GreekLetter *e)
{
  return boost::str(boost::format ("[%s]") % e->getName());
}

std::string TextRenderer::renderSymbol (const MDE_Symbol *e)
{
  switch (e->getSymbol()) {
  case MDE_Symbol::COMMA:
    return std::string ("<");

  case MDE_Symbol::FACTORIAL:
    return std::string ("!");

  case MDE_Symbol::THEREFORE:
    return std::string (" (Therefore) ");

  case MDE_Symbol::PERCENT:
    return std::string ("%");

  default:
    assert(false);
  }
}

std::string TextRenderer::renderRoot (const MDE_Root *e)
{
  std::string renderedIndex, renderedArgument;

  renderedIndex = renderFromVector (e->getIndex());
  renderedArgument = renderFromVector (e->getArgument());

  if (!renderedIndex.empty()) {
    return boost::str(boost::format("[Root Index='%s'] %s [End Root]") % renderedIndex % renderedArgument);
  } else {
    return boost::str(boost::format("[Sqrt] %s [End Sqrt]") % renderedArgument);
  }
   
}

std::string TextRenderer::renderFraction (const MDE_Fraction *e)
{
  std::string renderedNumerator, renderedDenominator;

  renderedNumerator = renderFromVector (e->getNumerator());
  renderedDenominator = renderFromVector (e->getDenominator());

  return boost::str(boost::format("[Fraction] (%s) over (%s) [End Fraction]") %  renderedNumerator % renderedDenominator);
}

std::string TextRenderer::renderExponent (const MDE_Exponent *e)
{
  std::string renderedExponent;

  renderedExponent = renderFromVector (e->getValue());

  return boost::str(boost::format("[Exponent] %s [End Exponent]") % renderedExponent);
}

std::string TextRenderer::renderSubscript (const MDE_Subscript *e)
{
  std::string renderedSubscript;

  renderedSubscript = renderFromVector (e->getValue());

  return boost::str(boost::format("[Sub] %s [End Sub]") % renderedSubscript);
}

std::string TextRenderer::renderUnsupported (const MathDocumentElement *e)
{
  return boost::str(boost::format("<? %s ?>") % typeid(*e).name());
}

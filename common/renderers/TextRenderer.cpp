/**
 * @file TextRenderer.cpp
 * Plain-text rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <assert.h>

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
  return boost::str(boost::format("«%s»") % e->getText());
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

std::string TextRenderer::renderUnsupported (const MathDocumentElement *e)
{
  return boost::str(boost::format("<? %s ?>") % typeid(*e).name());
}

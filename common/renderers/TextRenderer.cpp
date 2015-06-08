/**
 * @file TextRenderer.cpp
 * Plain-text rendering engine
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocument.h"
#include "MathExceptions.h"
#include "TextRenderer.h"

TextRenderer::TextRenderer (const MathDocument &md) : MathRenderer(md)
{
}

std::string TextRenderer::getOutput (void) const
{
  return output;
}

void TextRenderer::render (void)
{
  MathRenderer::render();
}

void TextRenderer::renderMathModeMarker (MDE_MathModeMarker *e)
{
  /* no output */
}

void TextRenderer::renderTextModeMarker (MDE_TextModeMarker *e)
{
  /* no output */
}

void TextRenderer::renderLineBreak (MDE_LineBreak *e)
{
  output += "\n";
}

void TextRenderer::renderTextBlock (MDE_TextBlock *e)
{
  output += e->getText();
}

void TextRenderer::renderMathBlock (MDE_MathBlock *e)
{
  output += "«";
  output += e->getText();
  output += "»";
}

void TextRenderer::renderUnsupported (MathDocumentElement *e)
{
  output += "<? ";
  output += typeid(*e).name();
  output += " ?>";
}

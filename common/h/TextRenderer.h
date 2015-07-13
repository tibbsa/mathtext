/**
 * @file MathRendererText.h
 * Header file for the plain text renderer.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __TEXT_RENDERER_H__
#define __TEXT_RENDERER_H__


#include <string>

#include "MathDocument.h"
#include "MathRenderer.h"

class TextRenderer : public MathRenderer
{
 protected:
  DECL_RENDER_FUNC(MathModeMarker);
  DECL_RENDER_FUNC(TextModeMarker);
  DECL_RENDER_FUNC(LineBreak);

  DECL_RENDER_FUNC(TextBlock);
  DECL_RENDER_FUNC(MathBlock);
  DECL_RENDER_FUNC(ItemNumber);
  DECL_RENDER_FUNC(Number);

  DECL_RENDER_FUNC(Operator);
  DECL_RENDER_FUNC(Comparator);
  DECL_RENDER_FUNC(GreekLetter);
  DECL_RENDER_FUNC(Symbol);

  DECL_RENDER_FUNC(Root);
  DECL_RENDER_FUNC(Fraction);
  
  DECL_RENDER_FUNC(Exponent);
  DECL_RENDER_FUNC(Subscript);

  virtual std::string renderUnsupported (const MathDocumentElement *e);

 public:
  TextRenderer (const MathDocument &md);
};


#endif /* __TEXT_RENDERER_H__ */

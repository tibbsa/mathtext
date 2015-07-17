/**
 * @file MathRenderer.h
 * Header file for the generic class that forms the basis of the rendering 
 * engines.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_RENDERER_H__
#define __MATH_RENDERER_H__

#ifndef __COMMON_INCLUDES_H__
#include "common-includes.h"
#endif

#include "MathDocument.h"

#define DECL_RENDER_FUNC(class) virtual std::string render##class (const MDE_##class *e)

class MathRenderer
{
 public:
  MathRenderer();

  DECL_RENDER_FUNC(SourceLine);
  DECL_RENDER_FUNC(Command);

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
  DECL_RENDER_FUNC(Modifier);

  DECL_RENDER_FUNC(Root);
  DECL_RENDER_FUNC(Fraction);

  DECL_RENDER_FUNC(Exponent);
  DECL_RENDER_FUNC(Subscript);

  virtual std::string renderElement (const MathDocumentElement *e);
  virtual std::string renderVector (const MDEVector &v);
  virtual std::string renderDocument (const MathDocument &document);
};


#endif /* __MATH_RENDERER_H__ */

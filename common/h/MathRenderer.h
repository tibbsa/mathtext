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

#include "MathDocument.h"

#define DECL_RENDER_FUNC(class) virtual void render##class (MDE_##class *e)

class MathRenderer
{
 protected:
  const MDEVector &doc;

  DECL_RENDER_FUNC(MathModeMarker);
  DECL_RENDER_FUNC(TextModeMarker);
  DECL_RENDER_FUNC(LineBreak);

  DECL_RENDER_FUNC(TextBlock);
  DECL_RENDER_FUNC(MathBlock);

  virtual void renderUnsupported (MathDocumentElement *e);

 public:
  MathRenderer (const MathDocument &md);

  virtual void render (void);
};


#endif /* __MATH_RENDERER_H__ */

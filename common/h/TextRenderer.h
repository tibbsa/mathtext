/**
 * @file MathRendererText.h
 * Header file for the plain text renderer.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_RENDERER_TEXT_H__
#define __MATH_RENDERER_TEXT_H__


#include <string>

#include "MathDocument.h"
#include "MathRenderer.h"

class TextRenderer : public MathRenderer
{
 protected:
  std::string output;

  DECL_RENDER_FUNC(MathModeMarker);
  DECL_RENDER_FUNC(TextModeMarker);
  DECL_RENDER_FUNC(LineBreak);

  DECL_RENDER_FUNC(TextBlock);
  DECL_RENDER_FUNC(MathBlock);

  virtual void renderUnsupported (MathDocumentElement *e);

 public:
  TextRenderer (const MathDocument &md);

  void render (void);
  std::string getOutput (void) const;
};


#endif /* __MATH_RENDERER_H__ */

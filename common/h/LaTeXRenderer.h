/**
 * @file LaTeXRenderer.h
 * Header file for the LaTeX renderer.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __LATEX_RENDERER_H__
#define __LATEX_RENDERER_H__

#include <string>

#include "MathDocument.h"
#include "MathRenderer.h"

class LaTeXRenderer : public MathRenderer
{
 private:
  std::string status (void) const;

  void setWriteMode (void);

 protected:
  typedef enum { UNKNOWN, MATH, TEXT } Mode;

  Mode writerLineMode;
  Mode writerCurrentMode;

  Mode currentBlockType;
  Mode currentSegmentType;

  bool isStartOfLine;

  std::string renderMathContent (const std::string &s);
  std::string renderTextContent (const std::string &s);
  
  DECL_RENDER_FUNC(SourceLine);

  DECL_RENDER_FUNC(MathModeMarker);
  DECL_RENDER_FUNC(TextModeMarker);
  DECL_RENDER_FUNC(LineBreak);

  DECL_RENDER_FUNC(TextBlock);
  DECL_RENDER_FUNC(MathBlock);
  DECL_RENDER_FUNC(ItemNumber);

  DECL_RENDER_FUNC(Operator);
  DECL_RENDER_FUNC(Comparator);
  DECL_RENDER_FUNC(GreekLetter);

  DECL_RENDER_FUNC(Fraction);

  DECL_RENDER_FUNC(Exponent);
  DECL_RENDER_FUNC(Subscript);

  virtual std::string renderUnsupported (const MathDocumentElement *e);

 public:
  LaTeXRenderer (const MathDocument &md);
};


#endif /* __LATEX_RENDERER_H__ */

/**
 * @file LaTeXRenderer.h
 * Header file for the LaTeX renderer.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __LATEX_RENDERER_H__
#define __LATEX_RENDERER_H__

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
  bool isBracketSizingEnabled;

  unsigned internalRenderCount;
  void beginInternalRender (void);
  bool doingInternalRender (void) const;
  void endInternalRender (void);

  std::string renderMathContent (const std::string &s);
  std::string renderTextContent (const std::string &s);

 public:
  LaTeXRenderer();

  std::string renderDocument (const MathDocument &document);

  static void getInterpreterCommandList (std::vector<std::string> &cmdlist);
  static std::string makeLaTeXSafe (const std::string &input);

  DECL_RENDER_FUNC(SourceLine);
  DECL_RENDER_FUNC(Command);

  DECL_RENDER_FUNC(MathModeMarker);
  DECL_RENDER_FUNC(TextModeMarker);
  DECL_RENDER_FUNC(LineBreak);

  DECL_RENDER_FUNC(TextBlock);
  DECL_RENDER_FUNC(MathBlock);
  DECL_RENDER_FUNC(Number);
  DECL_RENDER_FUNC(Group);

  DECL_RENDER_FUNC(ItemNumber);
  DECL_RENDER_FUNC(Operator);
  DECL_RENDER_FUNC(Comparator);
  DECL_RENDER_FUNC(GreekLetter);
  DECL_RENDER_FUNC(Symbol);
  DECL_RENDER_FUNC(Modifier);

  DECL_RENDER_FUNC(Root);
  DECL_RENDER_FUNC(Summation);
  DECL_RENDER_FUNC(Fraction);

  DECL_RENDER_FUNC(Exponent);
  DECL_RENDER_FUNC(Subscript);
};


#endif /* __LATEX_RENDERER_H__ */
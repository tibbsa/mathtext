/**
* @file
* @copyright Copyright (C) 2001-2015 Anthony Tibbs <anthony@tibbs.ca>
* \par License
* This project is released under the GNU Public License, version 3.
* See COPYING or http://www.gnu.org/licenses/ for more information.
*/

#ifndef __LATEX_RENDERER_H__
#define __LATEX_RENDERER_H__

#include "MathRenderer.h"

/** LaTeX rendering engine used to produce LaTeX (and then PDF) files.
*
* For the most part, this is a straightforward transformation from the
* math document to LaTeX.

* Two issues complicate this process.First, it is necessary to track
* the type of block we are currently in.On lines that appear to be text - based,
* math material is inserted as inline math($...$).On lines that appear to be
* primarily math - based, a full - blown math environment(\[... \]) is opened to
* allow for prettier displayed math material.
*
* Second, it is possible that pdflatex will ultimately report problems that
* cannot be automatically fixed : notably, lines may become too long and may
* not automatically wrap.This requires manual intervention on the part of the
* author to determine how the math on that line can be split up or otherwise
* simplified to span multiple lines.
*
* @todo Analysis of pdflatex output to recommend solutions not yet implemented!
*
* LaTeX packages depended on :
* - amsmath
* - amssymb
* - amstext
* - eurosym (for Euro currency symbols)
* - textcomp (for cents and Yen currency symbols)
* - wasysym
* - margin (to easily adjust page margins)
* - fancyhdr (for adding headers / footers)
* - lastpage (for calculating last page number in footer)
*/
class LaTeXRenderer : public MathRenderer
{
 private:
  std::string status (void) const;

 protected:
  /** 
   * Enum to identify which LaTeX output mode (math or text) we are in
   */
  typedef enum { 
    UNKNOWN, ///< Have not yet determined (start of line)
    MATH, ///< We are in a math block ($...$)
    TEXT ///< We are outputting text
   } Mode;

  /**
   * The type of line that we are currently writing (mostly math, mostly text).
   */
  Mode writerLineMode;

  /**
   * The mode that we last wrote in (math or text)
  */
  Mode writerCurrentMode;

  /**
   * Set to TRUE if we are at the beginning of a new line.
  */
  bool isStartOfLine;
  
  /**
   * If this feature is enabled, then \\left and \\right will be added to brackets 
   * in the LaTeX. This allows LaTeX to dynamically size the brackets to grow 
   * or shrink to fit what is between them -- but may cause problems for long 
   * expressions because it prevents any automatic wrapping.
   *
   * Default: Enabled
   * @see LaTeXRenderer::renderGroup
   */
  bool isBracketSizingEnabled;

  /**
   * Counts the rendering nesting level: for example, if rendering a numerator 
   * inside of a fraction, this would be one level of nesting. If rendering 
   * an exponent within a numerator within a fraction, this would be two 
   * levels of nesting. This is necessary to avoid changing render modes 
   * while inside a mathematical construct, for example.
   */
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

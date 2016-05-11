/**
 * @file UEBRenderer.h
 * Header file for the Unified English Braille Code renderer.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __UEB_RENDERER_H__
#define __UEB_RENDERER_H__

#include <stack>
#include <string>
#include "MathRenderer.h"

/**
 * Default line length for braille output; 0 disables wrapping altogether.
 * @see UEBRenderer::maxLineLength
 */
#define UEB_DEFAULT_LINE_LEN 0

// UEB math/text mode indicators
#define UEB_MATH_BLOCK_BEGIN "<|@M@|>"
#define UEB_TEXT_BLOCK_BEGIN "<|@T@|>"

// UEB wrapping markers
#define UEB_WORDWRAP_INDLEN  7
#define UEB_WORDWRAP_PRI1    "<|@1@|>"
#define UEB_WORDWRAP_PRI2    "<|@2@|>"
#define UEB_WORDWRAP_PRI3    "<|@3@|>"

// Technical braille symbols
#define UEB_CAPITAL_SIGN     BD_6
#define UEB_CONTINUATION     BD_5
#define UEB_DIRECTLY_ABOVE   BD_46 BD_35
#define UEB_DIRECTLY_BELOW   BD_46 BD_26
#define UEB_G1               BD_56
#define UEB_G1_WORD          UEB_G1 UEB_G1
#define UEB_G1_PASSAGE       UEB_G1 UEB_G1 UEB_G1
#define UEB_GROUP_BEGIN      BD_126
#define UEB_GROUP_END        BD_345
#define UEB_LEVEL_DOWN       BD_26
#define UEB_LEVEL_UP         BD_35
#define UEB_NUMBER_SIGN      BD_3456
#define UEB_NUMERIC_SPACE    BD_5
#define UEB_NUMERIC_MODE_SYMBOLS "ABCDEFGHIJ" UEB_COMMA UEB_PERIOD UEB_NUMERIC_SPACE

// General puncutation
#define UEB_COMMA            BD_2
#define UEB_DEGREES          BD_46 BD_245
#define UEB_LEFT_BRACE       BD_456 BD_126
#define UEB_LEFT_BRACKET     BD_46 BD_126
#define UEB_LEFT_PAREN       BD_5 BD_126
#define UEB_PERCENT          BD_46 BD_356
#define UEB_PERIOD           BD_256
#define UEB_RIGHT_BRACE      BD_456 BD_345
#define UEB_RIGHT_BRACKET    BD_46 BD_345
#define UEB_RIGHT_PAREN      BD_5 BD_345

// Math symbols
#define UEB_APPROX_EQUAL     BD_456 BD_35
#define UEB_DIV_SIGN         BD_5 BD_34
#define UEB_EQUAL_SIGN       BD_5 BD_2356
#define UEB_FACTORIAL        BD_235
#define UEB_FRAC_BEGIN       BD_12356
#define UEB_FRAC_DIVIDER     BD_46 BD_34
#define UEB_FRAC_END         BD_23456
#define UEB_GREATER_THAN     BD_4 BD_345
#define UEB_GREATER_THAN_EQ  BD_456 BD_4 BD_345
#define UEB_LESS_THAN        BD_4 BD_126
#define UEB_LESS_THAN_EQ     BD_456 BD_4 BD_126
#define UEB_MINUS_SIGN       BD_5 BD_36
#define UEB_PLUS_SIGN        BD_5 BD_235
#define UEB_ROOT_BEGIN       BD_146
#define UEB_ROOT_END         BD_346
#define UEB_SIMPLE_FRAC_DIVIDER  BD_34
#define UEB_SINCE            BD_4 BD_34
#define UEB_THEREFORE        BD_6 BD_16
#define UEB_TIMES_SIGN       BD_5 BD_236
#define UEB_UNEQUAL_SIGN     BD_5 BD_2356 BD_4 BD_156

// Over/under symbols
#define UEB_OVER_ARROW_RIGHT BD_45 BD_156
#define UEB_OVER_BAR         BD_156
#define UEB_OVER_DOT         BD_45 BD_256
#define UEB_OVER_HAT         BD_5 BD_156
#define UEB_UNDER_ARROW_RIGHT BD_6 BD_45 BD_156
#define UEB_UNDER_BAR        BD_6 BD_156
#define UEB_UNDER_DOT        BD_6 BD_45 BD_256
#define UEB_UNDER_HAT        BD_6 BD_5 BD_156

// Greek alphabet
#define UEB_GREEK_SIGN       BD_46
#define UEB_GREEK_ALPHA      "A"
#define UEB_GREEK_BETA       "B"
#define UEB_GREEK_GAMMA      "G"
#define UEB_GREEK_DELTA      "D"
#define UEB_GREEK_EPSILON    "E"
#define UEB_GREEK_ZETA       "Z"
#define UEB_GREEK_ETA        BD_156
#define UEB_GREEK_THETA      BD_1456
#define UEB_GREEK_IOTA       "I"
#define UEB_GREEK_KAPPA      "K"
#define UEB_GREEK_LAMBDA     "L"
#define UEB_GREEK_MU         "M"
#define UEB_GREEK_NU         "N"
#define UEB_GREEK_XI         "X"
#define UEB_GREEK_OMICRON    "O"
#define UEB_GREEK_PI         "P"
#define UEB_GREEK_RHO        "R"
#define UEB_GREEK_SIGMA      "S"
#define UEB_GREEK_TAU        "T"
#define UEB_GREEK_UPSILON    "U"
#define UEB_GREEK_PHI        "F"
#define UEB_GREEK_CHI        BD_12346
#define UEB_GREEK_PSI        "Y"
#define UEB_GREEK_OMEGA      "W"

// Currency symbols
#define UEB_CURRENCY_CENTS   BD_4 "C"
#define UEB_CURRENCY_EURO    BD_4 "E"
#define UEB_CURRENCY_FRANC   BD_4 "F"
#define UEB_CURRENCY_POUND   BD_4 "L"
#define UEB_CURRENCY_DOLLAR  BD_4 "S"
#define UEB_CURRENCY_YEN     BD_4 "Y"
#define UEB_CURRENCY_NAIRA   BD_4 "N"

/**
 * Unified English Braille rendering engine
 *
 * Renders a mathematical document into Unified English Braille Code, using 
 * the liblouis library where necessary for text-based content. 
 *
 * One of the more complicated aspects of this rendering is taking care of 
 * line breaks, to ensure that the mathematical statements are wrapped to 
 * new lines at appropriate places. To facilitate this, suitable wrapping 
 * points are marked in the interim output (UEB_WORDWRAP_PRIx macros), 
 * and then the entire document is word-wrapped in one pass at the end of
 * the render. 
 */
class UEBRenderer : public MathRenderer
{
 protected:
   /** 
    * Sets the maximum line length for each line of braille.
    * 
    * If this is set to 0, no wrapping will be done at all and each line of 
    * the input file will result in a line of braille.
    *
    * \par Default value:
    * UEB_DEFAULT_LINE_LEN (0), disabling word wrapping
    * @see UEBRenderer::disableLineWrapping
    * @see UEBRenderer::enableLineWrapping
    */
   unsigned maxLineLength;

   /**
    * Determine whether to put extra spaces around operators.
    *
    * Normally, no spaces are included on either side of mathematical 
    * operators (+ - / x). For lower grade levels and less experienced
    * braille readers, additional spacing may optionally be included.
    * Turn this on to add those extra spaces.
    *
    * \par Default value:
    * FALSE
    */
   bool isUsingSpacedOperators;

   /**
    * Render engine status flags.
    *
    * We need to keep track of various conditions (such as whether we are in
    * "numeric mode" (following a number sign), and these need to be saved 
    * and restored when rendering subexpressions (such as a numerator in a 
    * fraction). This structure is used to store these status flags.
    */
   struct UEBRenderStatus {
    bool isInTextBlock; ///< TRUE if we are inserting text material
    bool isNumericMode; ///< TRUE if we are in numeric mode (after a # sign)
    bool isStartOfLine; ///< TRUE if we are at the beginning of a new line
    
    /**
     * Set to TRUE if it is necessary to skip the next spaces seen.
     *
     * Sometimes when returning to an outer level from an internal rendering, 
     * it may be desirable to have any additional spaces that appear in the 
     * output bypassed. The setting of this variable will be propogated up 
     * the render status stack accordingly.
     *
     * @see UEBRenderer::endInternalRender
     */
    bool skipFollowingWhitespace; 
  };

  /**
   * The current status of the rendering engine.
   */
  UEBRenderStatus status;

  /**
   * A stack of saved status flags for the rendering engine.
   */
  std::stack<UEBRenderStatus> statusStack;

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

  std::string stripWrappingIndicators(const std::string &input) const;

  std::string translateToBraille (const std::string &s);
  std::string translateBrailleLetterIndicators (const std::string &s);
  std::string translateBraillePunctuation (const std::string &s);

  std::string renderMathContent (const std::string &s);
  std::string renderTextContent (const std::string &s);

 public:
  UEBRenderer();
  ~UEBRenderer();

  static void getInterpreterCommandList (std::vector<std::string> &cmdlist);

  static bool isBrailleItem (const MDEVector &v);
  static bool isSimpleFraction (const MDE_Fraction &frac);
  std::string renderDocument (const MathDocument &document);

  void disableLineWrapping (void);
  void enableLineWrapping (const unsigned length);
  bool isWrappingEnabled (void) const;

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


#endif /* __UEB_RENDERER_H__ */

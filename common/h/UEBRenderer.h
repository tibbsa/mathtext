/**
 * @file UEBRenderer.h
 * Header file for the Unified English Braille Code renderer.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __UEB_RENDERER_H__
#define __UEB_RENDERER_H__

#ifndef __COMMON_INCLUDES_H__
#include "common-includes.h"
#endif

#include "MathRenderer.h"

// Technical braille symbols
#define UEB_CAPITAL_SIGN     BD_6
#define UEB_G1               BD_56
#define UEB_G1_WORD          UEB_G1 UEB_G1
#define UEB_G1_PASSAGE       UEB_G1 UEB_G1 UEB_G1
#define UEB_GROUP_BEGIN      BD_126
#define UEB_GROUP_END        BD_345
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
#define UEB_APPROX_EQUAL     " " BD_456 BD_35 " "
#define UEB_DIV_SIGN         BD_5 BD_34
#define UEB_EQUAL_SIGN       " " BD_5 BD_2356 " "
#define UEB_FACTORIAL        BD_235
#define UEB_FRAC_BEGIN       BD_12356
#define UEB_FRAC_DIVIDER     BD_46 BD_34
#define UEB_FRAC_END         BD_23456
#define UEB_GREATER_THAN     " " BD_4 BD_345 " "
#define UEB_GREATER_THAN_EQ  " " BD_456 BD_4 BD_345 " "
#define UEB_GREEK_SIGN       BD_46
#define UEB_LESS_THAN        " " BD_4 BD_126 " "
#define UEB_LESS_THAN_EQ     " " BD_456 BD_4 BD_126 " "
#define UEB_LEVEL_DOWN       BD_26
#define UEB_LEVEL_UP         BD_35
#define UEB_MINUS_SIGN       BD_5 BD_36
#define UEB_PLUS_SIGN        BD_5 BD_235
#define UEB_ROOT_BEGIN       BD_146
#define UEB_ROOT_END         BD_346
#define UEB_SIMPLE_FRAC_DIVIDER  BD_34
#define UEB_SINCE            BD_4 BD_34
#define UEB_THEREFORE        BD_6 BD_16
#define UEB_TIMES_SIGN       BD_5 BD_236
#define UEB_UNEQUAL_SIGN     " " BD_5 BD_2356 BD_4 BD_156 " "

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


class UEBRenderer : public MathRenderer
{
 protected:
  typedef struct { 
    bool isNumericMode; 
    bool isStart;
    bool isUsingSpacedOperators;
  } UEBRenderStatus;

  UEBRenderStatus status;
  std::stack<UEBRenderStatus> statusStack;

  unsigned internalRenderCount;
  void beginInternalRender (void);
  bool doingInternalRender (void) const;
  void endInternalRender (void);

  std::string translateToBraille (const std::string &s);
  std::string translateBrailleLetterIndicators (const std::string &s);
  std::string translateBraillePunctuation (const std::string &s);
  bool isBrailleItem (const MDEVector &v) const;

  std::string renderMathContent (const std::string &s);
  std::string renderTextContent (const std::string &s);
  
 public:
  UEBRenderer();
  ~UEBRenderer();

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
};


#endif /* __UEB_RENDERER_H__ */

/**
 * @file
 * Header file for the class that holds a single element in a document.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_ELEMENTS_H__
#define __MATH_DOCUMENT_ELEMENTS_H__

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class MathDocumentElement;

/**
 * A pointer to a MathDocumentELement item.
 */
typedef boost::shared_ptr<MathDocumentElement> MathDocumentElementPtr;

/**
 * A vector (array) containing a series of MathDocumentElement items.
*/
typedef std::vector<MathDocumentElementPtr> MDEVector;

/**
 * A pointer to a vector (array) containing a series of MathDocumentElement items.
 */
typedef boost::shared_ptr<MDEVector> MDEVectorPtr;

/**
 * A MathDocumentElement holds a single item or element from the interpreted 
 * document: a number, a fraction, an operator sign, etc. A different type 
 * will be derived for each possible document element.
 *
 * @note Whenever a new type of document element is added, changes must also 
 *       be made to MathRenderer and in particular MathRenderer::renderElement.
 */
class MathDocumentElement
{
 protected:

 public:
  MathDocumentElement();
  ~MathDocumentElement();

  virtual std::string getString (void) const;
  friend std::ostream &operator<<(std::ostream &os, const MathDocumentElement &e);
};


/* ============== Internal/technical block types ======================= */

/**
 * A complete copy of the original source input line(s)
 */
class MDE_SourceLine : public MathDocumentElement
{
 protected:
   /** 
   * The name of the file from which this line was loaded
   */
  std::string filename;

  /** 
   * The starting line number that this expression began on.
   */
  unsigned long line1;
  
  /**
   * The ending line number that this expression finished on.
   */
  unsigned long line2;
  
  /**
   * The text contents of this line of the file.
   */
  std::string payload;

 public:
  MDE_SourceLine(const std::string &fname,
		 const unsigned long line_start,
		 const unsigned long line_end,
		 const std::string &contents);

  std::string getFilename (void) const;
  unsigned long getStartLine (void) const;
  unsigned long getEndLine (void) const;
  std::string getContents (void) const;

  virtual std::string getString (void) const;

};


/**
 * An "option", "feature", or "configuration" command
 */
class MDE_Command : public MathDocumentElement
{
 protected:
  std::string name;
  std::string parameters;

 public:
  MDE_Command(const std::string &cmdName, const std::string &cmdParam);
  const std::string &getName (void) const;
  const std::string &getParameters (void) const;

  virtual std::string getString (void) const;
};

/**
 * An internal marker used to indicate that the following material is
 * primarily mathematical or primary textual in nature.
 */
class MDE_MathModeMarker : public MathDocumentElement
{
 public:
  typedef enum { BLOCK_MARKER, SEGMENT_MARKER } MarkerType;

  MDE_MathModeMarker (const MarkerType mt = SEGMENT_MARKER);
  MarkerType getType (void) const;
  virtual std::string getString (void) const;

 protected:
  MarkerType type;
};

class MDE_TextModeMarker : public MathDocumentElement
{
 public:
  typedef enum { BLOCK_MARKER, SEGMENT_MARKER } MarkerType;

  MDE_TextModeMarker (const MarkerType mt = SEGMENT_MARKER);
  MarkerType getType (void) const;
  virtual std::string getString (void) const;

 protected:
  MarkerType type;

};

class MDE_LineBreak : public MathDocumentElement
{
 public:
  virtual std::string getString (void) const;
};


/**
 * A character (or set of characters) that should be treated as
 * plain text in a math or text context as-is.
 */
class MDE_GenericText : public MathDocumentElement
{
 protected:
  std::string payload;

 public:
  MDE_GenericText(const std::string &contents);

  void setText(const std::string &contents);
  const std::string &getText(void) const;

  virtual std::string getString (void) const;
};

/**
 * A character (or set of characters) that should be treated as
 * plain text in a math or text context as-is.
 */
class MDE_TextBlock : public MDE_GenericText
{
 public:
  MDE_TextBlock(const std::string &contents);

  virtual std::string getString (void) const;
};

/**
 * A character (or set of characters) that should be treated as
 * plain text in a math or text context as-is.
 */
class MDE_MathBlock : public MDE_GenericText
{
 public:
  MDE_MathBlock(const std::string &contents);

  virtual std::string getString (void) const;
};

/**
 * A number or set of digits.  Left-hand-side (whole numbers) and
 * right-hand-side (decimal portion) are separately stored in case a
 * special representation is needed in the output (e.g. EBAE braille).
 */
class MDE_Number : public MathDocumentElement
{
 public:
  typedef enum { NEGATIVE, POSITIVE } NumberType;

 public:
  MDE_Number (const MDE_Number::NumberType type, const std::string &whole, const std::string &decimals);
  bool isNegative (void) const;
  std::string getWholePortion (void) const;
  std::string getDecimalPortion (void) const;
  std::string getStandardNotation (void) const;

  virtual std::string getString (void) const;

 protected:
  NumberType numType;
  std::string lhs, rhs;

};

/**
 * A logical subgroup within an equation (bounded by parens, brackets, or
 * braces, as the case may be).
 */
class MDE_Group : public MathDocumentElement
{
 public:
  typedef enum { PARENTHESES, BRACKETS, BRACES } EnclosureType;

 protected:
  EnclosureType enclosure;
  MDEVector contents;

 public:
  MDE_Group(const EnclosureType encl, const MDEVector &content);

  MDEVector getContents (void) const;
  EnclosureType getType (void) const;

  virtual std::string getString (void) const;
};


/**
 * A question or item number (that appears at the start of a line)
 */
class MDE_ItemNumber : public MDE_TextBlock
{
 public:
  MDE_ItemNumber(const std::string &contents);

  virtual std::string getString (void) const;
};

/* ============== Operators, symbols, etc. ============================= */
/**
 * An operator (+/-*)
 */
class MDE_Operator : public MathDocumentElement
{
 public:
  typedef enum { ADDITION, SUBTRACTION, MULTIPLICATION, DIVISION } Operator;

  MDE_Operator (const Operator o);
  Operator getOperator (void) const;
  void setOperator (const Operator o);

  virtual std::string getString (void) const;

 protected:

  MDE_Operator::Operator op;
};

/**
 * A sign of comparision (< > = ~= <= >=)
 */
class MDE_Comparator : public MathDocumentElement
{
 public:
  typedef enum { LESS_THAN, GREATER_THAN, EQUALS, APPROX_EQUALS, NOT_EQUALS,
		 LESS_THAN_EQUALS, GREATER_THAN_EQUALS } Comparator;

  MDE_Comparator (const Comparator c);
  Comparator getComparator (void) const;
  void setComparator (const Comparator c);

  virtual std::string getString (void) const;

 protected:

  MDE_Comparator::Comparator comp;
};

/**
 * A greek letter (alpha, beta, gamma, etc.)
 */
class MDE_GreekLetter : public MathDocumentElement
{
 public:
  typedef enum { alpha, Alpha, beta, Beta, gamma, Gamma, delta, Delta,
		 epsilon, Epsilon, zeta, Zeta, eta, Eta, theta, Theta,
		 iota, Iota, kappa, Kappa, lambda, Lambda, mu, Mu, nu, Nu,
		 xi, Xi, omicron, Omicron, pi, Pi, rho, Rho,
		 sigma, Sigma, tau, Tau, upsilon, Upsilon,
		 phi, Phi, chi, Chi, psi, Psi, omega, Omega } Character;

  MDE_GreekLetter (const Character c);
  Character getValue (void) const;
  void setValue (const Character c);
  std::string getName (void) const;

  virtual std::string getString (void) const;

 protected:

  MDE_GreekLetter::Character letter;
};

/**
 * Various miscellaneous symbols that do not require particularly
 * special handling (do not have arguments or other values attached, etc.)
 */
class MDE_Symbol : public MathDocumentElement
{
 public:
  typedef enum {
    COMMA, // , (outside of a number)
    CURRENCY_CENTS, // ¢ (`C)
    CURRENCY_EURO,  // € (`E)
    CURRENCY_FRANC, // ₣ (`F)
    CURRENCY_POUND, // £ (`P)
    CURRENCY_DOLLAR, // $ (`$)
    CURRENCY_YEN,   // ¥ (`Y)
    FACTORIAL, // !
    LEFT_BRACE, // {
    LEFT_BRACKET, // [
    LEFT_PAREN, // (
    PERCENT, // %% (percent sign)
    PERIOD, // .
    RIGHT_BRACKET, // ]
    RIGHT_BRACE, // }
    RIGHT_PAREN, // )
    THEREFORE, // /\ (3 dot triangle)
  } Symbol;

  MDE_Symbol (const Symbol s);
  Symbol getSymbol (void) const;
  void setSymbol (const Symbol s);

  virtual std::string getString (void) const;

 protected:

  MDE_Symbol::Symbol symbol;
};

/**
 * A special symbol that has at least one argument attached to it,
 * typically for showing 'modified' symbols (e.g. with an arrow above,
 * or a hat over top, or a line below, or what have you).
 */
class MDE_Modifier : public MathDocumentElement
{
 public:
  typedef enum {
    OVER_ARROW_RIGHT, // (right-facing arrow above argment - vectors)
    OVER_BAR, // (horizontal bar above argument - conjugates, etc.)
    OVER_HAT, // (hat ^ above argment - unit vectors)
  } Modifier;

  MDE_Modifier (const Modifier m, const MDEVector arg);
  Modifier getModifier (void) const;
  MDEVector getArgument (void) const;

  virtual std::string getString (void) const;
  static const std::string &getModifierName (const Modifier m);

 protected:
  Modifier modifier;
  MDEVector argument;

};


/**
 * A root sign
 */
class MDE_Root : public MathDocumentElement
{
 protected:
  MDEVector index;
  MDEVector argument;

 public:
  MDE_Root(const MDEVector idx, const MDEVector contents);

  MDEVector getIndex(void) const;
  MDEVector getArgument(void) const;

  virtual std::string getString (void) const;
};

class MDE_Summation : public MathDocumentElement
{
 protected:
  MDEVector lowerBound;
  MDEVector upperBound;

 public:
  MDE_Summation(const MDEVector lower, const MDEVector upper);

  MDEVector getLowerBound (void) const;
  MDEVector getUpperBound (void) const;

  virtual std::string getString (void) const;
};


/* ============== Fractions ======================================= */
class MDE_Fraction : public MathDocumentElement
{
 protected:
  MDEVector numerator;
  MDEVector denominator;

 public:
  MDE_Fraction(const MDEVector num, const MDEVector den);

  MDEVector getNumerator (void) const;
  MDEVector getDenominator (void) const;

  virtual std::string getString (void) const;
};

/* ============== Exponents and Subscripts ======================== */
class MDE_Exponent : public MathDocumentElement
{
 protected:
  MDEVector contents;

 public:
  MDE_Exponent(const MDEVector exponent);

  MDEVector getValue (void) const;

  virtual std::string getString (void) const;
};

class MDE_Subscript : public MathDocumentElement
{
 protected:
  MDEVector contents;

 public:
  MDE_Subscript(const MDEVector subscript);

  MDEVector getValue (void) const;

  virtual std::string getString (void) const;
};

#endif /* __MATH_DOCUMENT_ELEMENTS_H__ */

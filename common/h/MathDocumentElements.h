/**
 * @file MathDocumentElements.h
 * Header file for the class that holds a single element in a document.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_ELEMENTS_H__
#define __MATH_DOCUMENT_ELEMENTS_H__

#include <vector>
#include <boost/shared_ptr.hpp>

class MathDocumentElement;
typedef boost::shared_ptr<MathDocumentElement> MathDocumentElementPtr;
typedef std::vector<MathDocumentElementPtr> MDEVector;
typedef boost::shared_ptr<MDEVector> MDEVectorPtr;

/**
 * A MathDocumentElement holds the contents of an interpreted math file.
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
  std::string filename;
  unsigned long line1;
  unsigned long line2;
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
 * An internal marker used to indicate that the following material is 
 * primarily mathematical or primary textual in nature.
 */
class MDE_MathModeMarker : public MathDocumentElement
{ public:   virtual std::string getString (void) const; };

class MDE_TextModeMarker : public MathDocumentElement
{ public:   virtual std::string getString (void) const; };

class MDE_LineBreak : public MathDocumentElement
{ public:   virtual std::string getString (void) const; };


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
 protected:
  bool negative;
  std::string lhs, rhs; 

 public:
  MDE_Number (const bool isNeg, const std::string &whole, const std::string &decimals);
  bool isNegative (void) const;
  std::string getWholePortion (void) const;
  std::string getDecimalPortion (void) const;
  std::string getStandardNotation (void) const;

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
    FACTORIAL, // !
    LEFT_BRACE, // {
    LEFT_BRACKET, // [ 
    LEFT_PAREN, // ( 
    PERCENT, // %% (percent sign) 
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

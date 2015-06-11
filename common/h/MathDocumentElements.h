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

  MDEVector getExponent (void) const;

  virtual std::string getString (void) const;
};

#endif /* __MATH_DOCUMENT_ELEMENTS_H__ */

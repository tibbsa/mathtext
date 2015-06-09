/**
 * @file MathDocumentElements.h
 * Header file for the class that holds a single element in a document.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_ELEMENTS_H__
#define __MATH_DOCUMENT_ELEMENTS_H__

#include <boost/shared_ptr.hpp>

/**
 * A MathDocumentElement holds the contents of an interpreted math file.
 */
class MathDocumentElement
{
 protected:
  boost::shared_ptr<MathDocumentElement> exponent;
  boost::shared_ptr<MathDocumentElement> subscript;

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
  MDE_GenericText();
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
  MDE_TextBlock();
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
  MDE_MathBlock();
  MDE_MathBlock(const std::string &contents);

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

#endif /* __MATH_DOCUMENT_ELEMENTS_H__ */

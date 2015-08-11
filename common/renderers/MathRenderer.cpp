/**
 * @file MathRenderer.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "logging.h"

#include "MathRenderer.h"
#include "MathExceptions.h"

MathRenderer::MathRenderer()
{
}


std::string MathRenderer::renderDocument (const MathDocument &document)
{
  return renderVector (document.getDocument());
}

std::string MathRenderer::renderVector (const MDEVector &v)
{
  std::string temp;

  std::string logtemp;
  BOOST_FOREACH (const MathDocumentElementPtr &ePtr, v) {
    const MathDocumentElement *ptr = ePtr.get();
    logtemp += ptr->getString();
  }
  LOG_TRACE << ">> renderVector(" << logtemp << ")";
  logIncreaseIndent();

  for (MDEVector::const_iterator it = v.begin();
       it != v.end(); ++it) {

    const MathDocumentElementPtr ePtr = *it;
    const MathDocumentElement *ptr = ePtr.get();
    temp += renderElement(ptr);
  }

  logDecreaseIndent();
  LOG_TRACE << "<< renderVector: " << temp;

  return temp;
}

#define RX(class) { if (const MDE_##class *ptr = dynamic_cast< const MDE_##class *>(e)) { return render##class (ptr); } }

std::string MathRenderer::renderElement (const MathDocumentElement *e)
{
  RX(SourceLine);
  RX(Command);

  RX(MathModeMarker);
  RX(TextModeMarker);
  RX(LineBreak);
  
  RX(ItemNumber); // have to do this before TextBlock as it is a subclass
  RX(TextBlock);
  RX(MathBlock);
  RX(Number);
  RX(Group);

  RX(Operator);
  RX(Comparator);
  RX(GreekLetter);
  RX(Symbol);
  RX(Modifier);

  RX(Root);
  RX(Summation);
  RX(Fraction);

  RX(Exponent);
  RX(Subscript);

  std::ostringstream os;
  os << "Unsupported element type in MathRenderer::renderElement(): " << typeid(*e).name();
  BOOST_THROW_EXCEPTION (MathRenderException() <<
			 mdx_error_info(os.str()));
  
}

#define PLACEHOLDER(class) \
  std::string MathRenderer::render##class (const MDE_##class *e) { \
    std::ostringstream os; \
    os << "Render callback not implemented in class " << typeid(*this).name() << " for " << typeid(*e).name(); \
    BOOST_THROW_EXCEPTION (MathRenderException() << \
                           mdx_error_info(os.str())); \
  } 

PLACEHOLDER(SourceLine);
PLACEHOLDER(Command);

PLACEHOLDER(MathModeMarker);
PLACEHOLDER(TextModeMarker);
PLACEHOLDER(LineBreak);

PLACEHOLDER(TextBlock);
PLACEHOLDER(MathBlock);
PLACEHOLDER(Group);
PLACEHOLDER(ItemNumber);
PLACEHOLDER(Number);

PLACEHOLDER(Operator);
PLACEHOLDER(Comparator);
PLACEHOLDER(GreekLetter);
PLACEHOLDER(Symbol);
PLACEHOLDER(Modifier);

PLACEHOLDER(Root);
PLACEHOLDER(Summation);
PLACEHOLDER(Fraction);

PLACEHOLDER(Exponent);
PLACEHOLDER(Subscript);


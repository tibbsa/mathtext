/**
 * @file MathRenderer.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <sstream>

#include "MathDocument.h"
#include "MathExceptions.h"
#include "MathRenderer.h"

MathRenderer::MathRenderer (const MathDocument &md) : doc(md.m_document)
{
}


std::string MathRenderer::render (void)
{
  return renderFromVector (doc);
}

std::string MathRenderer::renderFromVector (const MDEVector &v)
{
  std::string temp;

  for (MDEVector::const_iterator it = v.begin();
       it != v.end(); ++it) {

    const MathDocumentElementPtr ePtr = *it;
    const MathDocumentElement *ptr = ePtr.get();
    temp += renderElement(ptr);
  }

  return temp;
}

#define RX(class) { if (const MDE_##class *ptr = dynamic_cast< const MDE_##class *>(e)) { return render##class (ptr); } }

std::string MathRenderer::renderElement (const MathDocumentElement *e)
{
  RX(MathModeMarker);
  RX(TextModeMarker);
  RX(LineBreak);
  
  RX(ItemNumber); // have to do this before TextBlock as it is a subclass
  RX(TextBlock);
  RX(MathBlock);

  RX(Operator);
  RX(Comparator);
  RX(GreekLetter);

  RX(Fraction);

  RX(Exponent);
  RX(Subscript);

  std::ostringstream os;
  os << "Unsupported element type in MathRenderer::renderElement(): " << typeid(*e).name();
  BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			 mdx_error_info(os.str()));
  
}

#define PLACEHOLDER(class) \
  std::string MathRenderer::render##class (const MDE_##class *e)		\
  { return renderUnsupported (dynamic_cast<const MathDocumentElement*>(e)); }

PLACEHOLDER(MathModeMarker);
PLACEHOLDER(TextModeMarker);
PLACEHOLDER(LineBreak);

PLACEHOLDER(TextBlock);
PLACEHOLDER(MathBlock);
PLACEHOLDER(ItemNumber);

PLACEHOLDER(Operator);
PLACEHOLDER(Comparator);
PLACEHOLDER(GreekLetter);

PLACEHOLDER(Fraction);

PLACEHOLDER(Exponent);
PLACEHOLDER(Subscript);

std::string MathRenderer::renderUnsupported (const MathDocumentElement *e)
{
  std::ostringstream os;
  os << "Render callback not implemented for type " << typeid(*e).name();
  BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			 mdx_error_info(os.str()));
}

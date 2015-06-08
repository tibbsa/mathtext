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


#define RX(class) if (MDE_##class *e = dynamic_cast< MDE_##class *>(ptr)) { std::cout << "dfjiow: " << e << ", " << std::endl; render##class (e); continue; }
void MathRenderer::render (void)
{
  for (MDEVector::const_iterator it = doc.begin();
       it != doc.end(); ++it) {
    MathDocumentElementPtr ePtr = *it;
    MathDocumentElement *ptr = ePtr.get();

    RX(MathModeMarker);
    RX(TextModeMarker);
    RX(LineBreak);

    RX(TextBlock);
    RX(MathBlock);

    std::ostringstream os;
    os << "Unsupported element type in MathRenderer::render(): " << typeid(*ptr).name();
    BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			   mdx_error_info(os.str()));
  }
}

#define PLACEHOLDER(class) \
  void MathRenderer::render##class (MDE_##class *e)\
  { renderUnsupported (dynamic_cast<MathDocumentElement*>(e)); }

PLACEHOLDER(MathModeMarker);
PLACEHOLDER(TextModeMarker);
PLACEHOLDER(LineBreak);

PLACEHOLDER(TextBlock);
PLACEHOLDER(MathBlock);

void MathRenderer::renderUnsupported (MathDocumentElement *e)
{
  std::ostringstream os;
  os << "Render callback not implemented for type " << typeid(*e).name();
  BOOST_THROW_EXCEPTION (MathDocumentRenderException() <<
			 mdx_error_info(os.str()));
}

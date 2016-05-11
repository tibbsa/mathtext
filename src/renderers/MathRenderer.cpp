/**
* @file
* @copyright Copyright (C) 2001-2015 Anthony Tibbs <anthony@tibbs.ca>
* \par License
* This project is released under the GNU Public License, version 3. 
* See COPYING or http://www.gnu.org/licenses/ for more information. 
*/

#include <boost/foreach.hpp>

#include "logging.h"

#include "MathDocument.h"
#include "MathRenderer.h"
#include "MathExceptions.h"

/**
 * Basic constructor.
 */
MathRenderer::MathRenderer()
{
}

/**
 * Render the provided document into an output text string.
  *
 * @param document Interpreted math document to render
 * @return std::string containing the rendered output of the document
 */
std::string MathRenderer::renderDocument (const MathDocument &document)
{
  return renderVector (document.getDocument());
}

/**
 * Render a particular vector/set of elements to an output text string.
 *
 * Loops through each MathDocumentElement in the provided vector and 
 * renders each, concatenating them together into the output string 
 * that is returned. 
 *
 * @param [in] v A vector (array) of MathDocumentElement elements to render
 * @return std::string containing the rendered output of the vector
 */
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


/** 
 * Helper macro that takes care of attempting to reflect a given element to a particular 
 * rendering function.
 * 
 * @param class The element type that we are attempting to reflect to.
 * @internal
 */
#define RX(class) { if (const MDE_##class *ptr = dynamic_cast< const MDE_##class *>(e)) { return render##class (ptr); } }

/**
 * Renders the specified MathDocumentElement to a text string.
 * 
 * Accepts a pointer to a MathDocumentElement item, determines what type of element 
 * has been provided, and calls the appropriate rendering function for that 
 * type of element.
 *
 * @param [in] e Pointer to a MathDocumentElement item to be rendered.
 * @return std::string containing the rendered element.
 * @throw MathRenderException is thrown if renderElement() is called with a 
 *        MathDocumentElement type that is not recognized.
 * @note Whenever a new MathDocumentElement type is created, a new reflector must
 *       be added here (and render* functions added, etc.)!
  */
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

/**
 * @file
 * Implementation of the container holding the interpreted source document
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocument.h"

/**
 * Sets up a new, blank math document
 */
MathDocument::MathDocument()
{
}

/**
 * Retrieve a (constant) reference to the document.
 *
 * @return A const reference to the document vector.
 */
const MDEVector &MathDocument::getDocument (void) const
{
  return m_document;
}

/**
 * Adds an element to the current document. Expects to receive the element 
 * as a pointer, which is then added to the document collection.
 *
 * @param [in] element Pointer to a MathDocumentElement to add to document
 */
void MathDocument::addElementPtr (MathDocumentElementPtr element)
{
  m_document.push_back (element);
}

/**
 * Adds a series of elements to the document. Expects to receive a vector 
 * of element pointers.
 *
 * @param [in] elements A vector of element pointers, each of which will be 
 *             added to the current document.
 */
void MathDocument::addElements (const MDEVector &elements)
{
  m_document.insert(m_document.end(), elements.begin(), elements.end());
}

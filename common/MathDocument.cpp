/**
 * @file MathDocument.cpp
 * Implementation of the container holding the interpreted source document
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocument.h"

/**
 * Sets up a new, blank math document
 */
MathDocument::MathDocument()
{
}

const MDEVector &MathDocument::getDocument (void) const
{
  return m_document;
}

void MathDocument::addElementPtr (MathDocumentElementPtr element)
{
  m_document.push_back (element);
}

void MathDocument::addElements (const MDEVector &elements)
{
  m_document.insert(m_document.end(), elements.begin(), elements.end());
}

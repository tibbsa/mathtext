/**
 * @file MathDocument.h
 * Header file for the class that holds interpreted math documents
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_DOCUMENT_H__
#define __MATH_DOCUMENT_H__

#include "MathDocumentElements.h"


/**
 * A MathDocument holds the contents of an interpreted math file./
 */

class MathDocument
{
 protected:
  MDEVector m_document;

 public:
  MathDocument();

  const MDEVector &getDocument (void) const;
  void addElementPtr (MathDocumentElementPtr element);
  void addElements (const MDEVector &elements);
};

#endif /* __MATH_DOCUMENT_H__ */

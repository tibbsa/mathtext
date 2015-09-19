/**
* @file
* @copyright Copyright (C) 2001-2015 Anthony Tibbs <anthony@tibbs.ca>
* \par License
* This project is released under the GNU Public License, version 3.
* See COPYING or http://www.gnu.org/licenses/ for more information.
*/


#ifndef __MATH_RENDERER_H__
#define __MATH_RENDERER_H__

#include "MathDocumentElements.h"

class MathDocument;

/**
 * Helper macro to quickly define virtual render functions for each element type.
 *
 * @param [in] class Name of the MathDocumentElement-derived class type
 * @internal
 */
#define DECL_RENDER_FUNC(class) std::string render##class (const MDE_##class *e)

/**
 * An abstract base class providing the framework for rendering math
 * documents to external formats. The base implementation takes care of looping 
 * through the document and calling render* methods as required.
 *
 * @note If new element types are added, new DECL_RENDER_FUNC() lines are needed 
 *       here as well!
 */
class MathRenderer
{
public:
  MathRenderer();

  /**
   * Render callback for MDE_SourceLine elements.
   * 
   * @param [in] e Source element to be rendered
   * @return std::string containing a fully rendered version of the element
   */
  virtual DECL_RENDER_FUNC(SourceLine) = 0;

  /**
  * Render callback for MDE_Command elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  * @note "Command" elements typically change interpreter/rendering behaviour but
  *       may not in and of themselves generate output.
  */
  virtual DECL_RENDER_FUNC(Command) = 0;

  /**
  * Render callback for MDE_MathModeMarker elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  * @note Mode marker elements typically change interpreter/rendering behaviour but
  *       may not in and of themselves generate output.
  */
  virtual DECL_RENDER_FUNC(MathModeMarker) = 0;

  /**
  * Render callback for MDE_TextModeMarker elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  * @note Mode marker elements typically change interpreter/rendering behaviour but
  *       may not in and of themselves generate output.
  */
  virtual DECL_RENDER_FUNC(TextModeMarker) = 0;

  /**
  * Render callback for MDE_LineBreak elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(LineBreak) = 0;

  /**
  * Render callback for MDE_TextBlock elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(TextBlock) = 0;

  /**
  * Render callback for MDE_MathBlock elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(MathBlock) = 0;

  /**
  * Render callback for MDE_Number elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Number) = 0;

  /**
  * Render callback for MDE_Group elements.
  *
  * Render a group of elements, enclosed by a grouping symbol -- ([{ }]) -- 
  * including rendering of any enclosed or nested elements. 
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Group) = 0;

  /**
  * Render callback for MDE_ItemNumber elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(ItemNumber) = 0;

  /**
  * Render callback for MDE_OperatorNumber elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Operator) = 0;

  /**
  * Render callback for MDE_Comparator elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Comparator) = 0;

  /**
  * Render callback for MDE_GreekLetter elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(GreekLetter) = 0;

  /**
  * Render callback for MDE_Symbol elements.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Symbol) = 0;

  /**
  * Render callback for MDE_Modifier elements.
  *
  * Render a group of elements, which is subject to a modification symbol 
  * of some sort (e.g. vector arrows above, hats/circumflex above, etc.)
  * The expectation is that both the "modified" element as well as the 
  * applicable modifiers will be rendered by this function.
  * 
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Modifier) = 0;

  /**
  * Render callback for MDE_Root elements.
  *
  * Renders a root symbol, including all elements which are under the root.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Root) = 0;

  /**
  * Render callback for MDE_Summation elements.
  *
  * Renders a summation (sigma) symbol, including any lower/upper bounds.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Summation) = 0;

  /**
  * Render callback for MDE_Fraction elements.
  *
  * Renders a fraction, including rendering of the contents of both the 
  * numerator and denominator accordingly.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Fraction) = 0;

  /**
  * Render callback for MDE_Exponent elements.
  *
  * Renders an exponent or superscript, including rendering of all items that 
  * are in the exponent/superscript.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Exponent) = 0;

  /**
  * Render callback for MDE_Subscript elements.
  *
  * Renders a subscript, including rendering of all items that
  * are in the subscript itself.
  *
  * @param [in] e Source element to be rendered
  * @return std::string containing a fully rendered version of the element
  */
  virtual DECL_RENDER_FUNC(Subscript) = 0;
    
  virtual std::string renderDocument(const MathDocument &document);
  virtual std::string renderVector(const MDEVector &v);
  virtual std::string renderElement(const MathDocumentElement *e);
};

#endif /* __MATH_RENDERER_H__ */

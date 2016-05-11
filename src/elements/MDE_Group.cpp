/**
 * @file MDE_Group.cpp
 * A logical grouping of elements (encompassed with paranthesees, brackets, or
 * braces as the case may be) within a mathematical expression.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Group::MDE_Group(const MDE_Group::EnclosureType encl,
		     const MDEVector &content) :
  enclosure(encl), contents(content)
{
}

MDEVector MDE_Group::getContents (void) const
{
  return contents;
}

MDE_Group::EnclosureType MDE_Group::getType (void) const
{
  return enclosure;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Group::getString (void) const
{
  std::string output;
  char groupOpen, groupClose;
  if (enclosure == MDE_Group::PARENTHESES) {
    groupOpen = '(';
    groupClose = ')';
  } else if (enclosure == MDE_Group::BRACKETS) {
    groupOpen = '[';
    groupClose = ']';
  } else if (enclosure == MDE_Group::BRACES) {
    groupOpen = '{';
    groupClose = '}';
  }
  else
    assert (0);

  output = "<GROUP:" + std::string(1, groupOpen) + ">";
  for (MDEVector::const_iterator it = contents.begin();
       it != contents.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }

  output += "</GROUP:" + std::string(1, groupClose) + ">";
  return output;
}



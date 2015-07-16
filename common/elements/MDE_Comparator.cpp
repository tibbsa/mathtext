/**
 * @file MDE_Operator.cpp
 * Signs of comparison (< <= > >= != ~=)
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Comparator::MDE_Comparator(const MDE_Comparator::Comparator c) : comp(c)
{
}

MDE_Comparator::Comparator MDE_Comparator::getComparator (void) const 
{
  return comp;
}

void MDE_Comparator::setComparator (const MDE_Comparator::Comparator c)
{
  comp = c;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Comparator::getString (void) const
{
  std::string output;

  if (comp == LESS_THAN)
    output = "[ < ]";
  else if (comp == GREATER_THAN)
    output = "[ > ]";
  else if (comp == LESS_THAN_EQUALS)
    output = "[<=]";
  else if (comp == GREATER_THAN_EQUALS)
    output = "[ >= ]";
  else if (comp == EQUALS)
    output = "[ = ]";
  else if (comp == APPROX_EQUALS)
    output = "[ ~= ]";
  else if (comp == NOT_EQUALS)
    output = "[ <> ]";
  else
    assert (false);

  return output;
}



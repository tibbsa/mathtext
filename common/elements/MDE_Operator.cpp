/**
 * @file MDE_Operator.cpp
 * Signs of operation (+ - * /)
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Operator::MDE_Operator(const MDE_Operator::Operator o) : op(o)
{
}

MDE_Operator::Operator MDE_Operator::getOperator (void) const
{
  return op;
}

void MDE_Operator::setOperator (const MDE_Operator::Operator o)
{
  op = o;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Operator::getString (void) const
{
  std::string output;

  if (op == ADDITION)
    output = "<plus>";
  else if (op == SUBTRACTION)
    output = "<minus>";
  else if (op == MULTIPLICATION)
    output = "<mult>";
  else if (op == DIVISION)
    output = "<div>";
  else
    assert (false);

  return output;
}



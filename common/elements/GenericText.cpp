/**
 * @file MDE_GenericText.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "logging.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_GenericText::MDE_GenericText(const std::string &contents) : payload(contents)
{
}

void MDE_GenericText::setText (const std::string &contents)
{
  payload = contents;
}

const std::string &MDE_GenericText::getText (void) const
{
  return payload;
}


/**
 * Generate a string representation of this element (primarily for 
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_GenericText::getString (void) const
{
  std::string output;

  output = "<GENERIC>" + payload + MathDocumentElement::getString() + "</GENERIC>";

  return output;
}


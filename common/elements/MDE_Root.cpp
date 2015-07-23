/**
 * @file MDE_Root.cpp
 * Root (square or otherwise) symbol
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Root::MDE_Root(const MDEVector idx, const MDEVector contents) : 
  index(idx), argument(contents)
{
}

MDEVector MDE_Root::getIndex (void) const 
{
  return index;
}

MDEVector MDE_Root::getArgument (void) const 
{
  return argument;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Root::getString (void) const
{
  std::string output;

  output = "<ROOT";

  if (index.size()) {
    output += " INDEX:";

    for (MDEVector::const_iterator it = index.begin();
	 it != index.end(); ++it) {
      MathDocumentElementPtr e = *it;
      output += e->getString();
    }
  }
  output += ">";

  for (MDEVector::const_iterator it = argument.begin();
       it != argument.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }

  output += "</ROOT>";
  return output;
}



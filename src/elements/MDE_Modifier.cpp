/**
 * @file MDE_Modifier.cpp
 * Various symbols and presentation items that are attached to an argument,
 * such as vector symbols (right arrow over item), conjugates, etc., but which
 * do not otherwise require very special handling (such as roots, etc.).
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <map>
#include <boost/assign.hpp>
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Modifier::MDE_Modifier(const MDE_Modifier::Modifier m,
			   const MDEVector arg) : modifier(m), argument(arg)
{
}

MDE_Modifier::Modifier MDE_Modifier::getModifier (void) const
{
  return modifier;
}

MDEVector MDE_Modifier::getArgument (void) const
{
  return argument;
}

const std::string &MDE_Modifier::getModifierName (const Modifier m)
{
#define MAP(m) (m, #m)
  static std::map<MDE_Modifier::Modifier,std::string> map = boost::assign::map_list_of
    MAP(OVER_ARROW_RIGHT)
    MAP(OVER_BAR)
    MAP(OVER_HAT)
    ;
#undef MAP

  assert (map.count(m) == 1);
  return map[m];

}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Modifier::getString (void) const
{
  std::string output;

  output = "<" + getModifierName(modifier) + ">";
  for (MDEVector::const_iterator it = argument.begin();
       it != argument.end(); ++it) {
    MathDocumentElementPtr e = *it;
    output += e->getString();
  }
  output += "</" + getModifierName(modifier) + ">";

  return output;
}



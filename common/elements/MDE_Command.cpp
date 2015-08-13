/**
 * @file MDE_MDE_Command.cpp
 * Processing, rendering, and other configuration commands
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_Command::MDE_Command(const std::string &cmdName, const std::string &cmdParam) : name(cmdName), parameters(cmdParam)
{
}

const std::string &MDE_Command::getName (void) const
{
  return name;
}

const std::string &MDE_Command::getParameters (void) const
{
  return parameters;
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_Command::getString (void) const
{
  std::string output;

  if (!parameters.empty())
    output = boost::str(boost::format("<cmd '%s', arg='%s'>") % name % parameters);
  else
    output = boost::str(boost::format("<cmd '%s'>") % name);

  return output;
}



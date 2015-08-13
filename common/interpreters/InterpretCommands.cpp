/**
 * @file InterpretCommands.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a 'command'.  These appear in the file as
 * $$[command-name] [options]
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 *
 * If the commands are instructions to the interpreter, these are processed
 * frmo this function as well.
 */
bool MathInterpreter::interpretCommand (MDEVector &target,
					const std::string &src,
					size_t &i)
{
  // We should only be attempting this if this is the very start of a line of
  // text.
  assert (m_isStartOfLine == true);

  if (src.substr(i, 2) != "$$")
    return false;

  i += 2;
  std::string commandName, commandParameters;

  // Handle $$+EnableMoreFeatures and $$-EnableMoreFeatures.  Include a
  // trailing space in case more parameters show up below
  if (src [i] == '+') {
    commandParameters = "true ";
    i++;
  } else if (src [i] == '-') {
    commandParameters = "false ";
    i++;
  }

  // Grab the command name
  while (i < src.length() && isalpha(src [i])) {
    commandName += src[i];
    i++;
  }

  // Grab the parameters, if there are any, to EOL.  If there is already
  // a parameter there, add a space after it.
  while (i < src.length()) {
    commandParameters += src[i];
    i++;
  }
  boost::trim(commandParameters);

  if (!isCommand(commandName)) {
    LOG_TRACE << "* found unrecognized command: " << commandName << " // " << commandParameters;
    MSG_ERROR(UNKNOWN_COMMAND, boost::str(boost::format("'%s'") % commandName));
    BOOST_THROW_EXCEPTION (MathInterpreterException());

  }
  LOG_TRACE << "* found command: " << commandName << " // " << commandParameters;

  MathDocumentElementPtr e = boost::make_shared<MDE_Command>(commandName, commandParameters);

  target.push_back (e);
  return true;
}

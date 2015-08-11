/**
 * @file InterpretGroups.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a 'group' -- something enclosed in ([{ }]) 
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
bool MathInterpreter::interpretGroup (MDEVector &target,
				      const std::string &src, 
				      size_t &i)
{
  std::string contents;
  MDE_Group::EnclosureType groupType;

  if (src[i] == '(') {
    if (!extractGroup(contents, src, i, "(", ")")) {
      MSG_ERROR(GROUP_NOT_TERMINATED, boost::str(boost::format("end of line was reached while looking for closing paranthesis - saw %s") % contents));

      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }

    groupType = MDE_Group::PARENTHESES;
    LOG_TRACE << "* found paranthetical group: (" << contents << ")";
  }
  else if (src [i] == '[') {
    if (!extractGroup(contents, src, i, "[", "]")) {
      MSG_ERROR(GROUP_NOT_TERMINATED, boost::str(boost::format("end of line was reached while looking for closing bracket - saw %s") % contents));

      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }

    groupType = MDE_Group::BRACKETS;
    LOG_TRACE << "* found bracketed group: [" << contents << "]";
  }
  else if (src [i] == '{') {
    if (!extractGroup(contents, src, i, "{", "}")) {
      MSG_ERROR(GROUP_NOT_TERMINATED, boost::str(boost::format("end of line was reached while looking for closing paranthesis - saw %s") % contents));

      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }


    groupType = MDE_Group::BRACES;
    LOG_TRACE << "* found braced group: {" << contents << "}";
  }
  else
    return false;

  MDEVector contents_vec;
  contents_vec = interpretBuffer (contents);
  
  target.push_back (boost::make_shared<MDE_Group>(groupType, contents_vec));
  return true;
}

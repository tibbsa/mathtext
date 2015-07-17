/**
 * @file InterpretModifiers.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"
#include "MathExceptions.h"

/**
 * Attempts to interpret a variety of "modifiers" or symbols which have 
 * arguments attached (such as vectors, etc.)
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
namespace {
  struct ModifierMap { 
    ModifierMap (const std::string &s, const std::string &name, const MDE_Modifier::Modifier mod) : search(s), modifierName(name), modifier(mod) {}
    std::string search;
    std::string modifierName;
    MDE_Modifier::Modifier modifier;
  };
}

bool MathInterpreter::interpretModifier (MDEVector &target,
				      const std::string &src,
				      size_t &i)
{

  // Quick check: don't bother with all this if the first character doesn't
  // make this at least a possibility.
  if (src[i] != '`')
    return false;

#define MAX_MODIFIER_LEN 4
  // Be sure to update MAX_MODIFIER_LEN if longer symbols get added to
  // this table.  Place longer symbols at the top.
  static const std::vector<ModifierMap> map = boost::assign::list_of 
    ( ModifierMap("`V", "vector", MDE_Modifier::OVER_ARROW_RIGHT) )
    ( ModifierMap("`BAR", "bar", MDE_Modifier::OVER_BAR) )
    ( ModifierMap("`CJ", "conjugate", MDE_Modifier::OVER_BAR) )
    ( ModifierMap("`HAT", "hat", MDE_Modifier::OVER_HAT) )
    ( ModifierMap("`H", "hat", MDE_Modifier::OVER_HAT) )
    ;

  const std::string temp = src.substr(i, MAX_MODIFIER_LEN);
  
  for(std::vector<ModifierMap>::const_iterator it = map.begin(); 
      it != map.end(); ++it) {
    const ModifierMap &mi = *it;
  
    if (!strBeginsWith(temp, mi.search))
      continue;

    // Skip over symbol indicator
    i += mi.search.length();

    // skip any whitespace
    while (i < src.length() && isspace(src[i]))
      i++;

    if (i == src.length()) {
      MSG_ERROR(MODIFIER_MISSING_ARGUMENT, boost::str(boost::format("%s symbol") % mi.modifierName));
      BOOST_THROW_EXCEPTION (MathInterpreterException());
    }

    // If this is followed by an open paren '(', read until the closing 
    // paren
    std::string argument;
    if (src [i] == '(') {
      if (!extractGroup(argument, src, i)) {
	MSG_ERROR(MODIFIER_NOT_TERMINATED, boost::str(boost::format("text found inside %s symbol so far: '%s'") % mi.modifierName % argument));
	BOOST_THROW_EXCEPTION (MathInterpreterException());
      }
    } else if (src [i] == '@') {
      if (!extractGroup(argument, src, i, "@", "#", true)) {
	MSG_ERROR(MODIFIER_NOT_TERMINATED, boost::str(boost::format("partial fraction inside %s symbol so far: '%s'") % mi.modifierName % argument));
	BOOST_THROW_EXCEPTION (MathInterpreterException());
      }
    } else {
      extractItem(argument, src, i);
    }
    
    LOG_TRACE << "* found item in " << mi.modifierName << " symbol: " << argument;
    MDEVector v;
    v = interpretBuffer (argument);
    target.push_back (boost::make_shared<MDE_Modifier>(mi.modifier, v));
    return true;
  }

  return false;
}

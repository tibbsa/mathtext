/**
 * @file InterpretNumbers.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"

/**
 * Attempts to interpret a number or set of digits.
 *
 * A number may follow the form:
 * 1   1.1   .1   -1   -1.1   -.1
 * 1,024  2,048,576   4 122 133
 * All are valid.
 */
bool MathInterpreter::interpretNumber (MDEVector &target,
				    const std::string &src, 
				    size_t &i)
{
  size_t pos = i;
  int curDigitGroupCount = 0;
  MDE_Number::NumberType numType;

  if (src [pos] == '-') {
    numType = MDE_Number::NEGATIVE;
    pos++;
  }
  else
    numType = MDE_Number::POSITIVE;

  // Case 1: .24 (decmial with no leading numbers)
  // Allow numeric spaces here if escaped (\ ) but not commas.
  if (src [pos] == '.') {
    if (pos <= (src.length() - 1)) {
      if (isdigit (src [pos+1])) {
	std::string rhs;
	pos++;

	while (pos < src.length()) {
	  if (isdigit (src [pos])) {
	    rhs += src.substr(pos, 1);
	    pos++;
	  } else if (src.substr(pos, 2) == "\\ ") {
	    rhs += " ";
	    pos += 2;
	  }
	  else
	    break;
	}
	
	LOG_TRACE << "* adding decimal number w/o whole portion: (neg=" << ((numType == MDE_Number::NEGATIVE) ? 1 : 0) << ") " << rhs;
	target.push_back (boost::make_shared<MDE_Number>(numType, std::string(), rhs));

	i = pos;
	return true;
      }
    }

    return false;
  }

  // Case 2: 121 (plain old number)
  if (isdigit (src[pos])) {
    std::string lhs, rhs;

    // Left hand side: allow numeric spaces (\ ) and comma separations of
    // thousands.
    while (pos < src.length()) {
      if (isdigit(src [pos])) {
	curDigitGroupCount++;
 	lhs += src.substr(pos, 1);
	pos++;
	continue;
      }
      else if (isOneOf (src[pos], ", ")) {
	if (curDigitGroupCount <= 3 && (src.length() - pos) >= 3) {
	  // If we see "," or " " followed by 3 digits and then a non-digit, 
	  // assume this is a thousands separator within a number
	  boost::regex re("^[, ]\\d{3}");
	  boost::smatch matches;
	  if (boost::regex_search(src.substr(pos, 4), matches, re)) {
	    // If the next character is NOT a number, we assume this 
	    // was a set of thousands and add it in.  If we see yet 
	    // another number, then this was not a thousands separator
	    // and we abort.
	    // Example when we continue: 1,024,576
	    // Example when we do not: 1,24837,23872 (treat as separate)
	    if ((src.length() - pos == 3) || !isdigit(src [pos+4])) {
	      lhs += src.substr(pos, 4);
	      pos += 4;
	      curDigitGroupCount = 0;
	    }
	    else
	      break;
	  }
	  else
	    break;
	}
	else
	  break;
      } else if (src.substr(pos, 2) == "\\ ") {
	// End number here
	pos++;
	break;
      }
      else
	break;
    }
    
    // Case 3: 121.25 (with decmial portion)
    if (pos < src.length() && src [pos] == '.') {
      pos++;
      curDigitGroupCount = 0;

      while (pos < src.length()) {
	if (isdigit (src [pos])) {
	  curDigitGroupCount++;
	  rhs += src.substr(pos, 1);
	  pos++;
	} else if (src[pos] == ' ') {
	  if (curDigitGroupCount <= 3 && (src.length() - pos) >= 3) {
	    // If we see " " followed by 3 digits and then a non-digit, 
	    // assume this is a thousands separator within a number
	    boost::regex re("^ \\d{3}");
	    boost::smatch matches;
	    if (boost::regex_search(src.substr(pos, 4), matches, re)) {
	      // If the next character is NOT a number, we assume this 
	      // was a set of thousands and add it in.  If we see yet 
	      // another number, then this was not a thousands separator
	      // and we abort.
	      // Example when we continue: 1,024,576
	      // Example when we do not: 1,24837,23872 (treat as separate)
	      if ((src.length() - pos == 3) || !isdigit(src [pos+4])) {
		rhs += src.substr(pos, 4);
		pos += 4;
		curDigitGroupCount = 0;
	      }
	      else
		break;
	    }
	    else
	      break;
	  }
	  else
	    break;
	} else if (src.substr(pos, 2) == "\\ ") {
	  // End number here
	  pos++;
	  break;
	}
	else
	  break;
      }
    }

    LOG_TRACE << "* adding decimal number: (neg=" << ((numType == MDE_Number::NEGATIVE) ? 1 : 0) << ") " << lhs << "." << rhs;
    target.push_back (boost::make_shared<MDE_Number>(numType, lhs, rhs));

    i = pos;
    return true;
  }

  return false;
}

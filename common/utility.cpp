/**
 * @file utility.cpp
 * Various miscellaneous utility functions that do not fit elsewhere.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <string>

#include <boost/algorithm/string.hpp>

#include "utility.h"

/**
 * Removes an existing file extension from a string.
 *
 * @param std::string Filename to remove extension from
 * @return Modified filename with extension removed
 */
std::string remove_file_extension (const std::string &filename)
{
  size_t lastDot = filename.find_last_of(".");
  if (lastDot == std::string::npos)
    return filename;
  else
    return filename.substr(0, lastDot);
}

/**
 * Determine if the subject string begins (case-insensitive)  with search phrase.
 *
 * @param subject String to be searched
 * @param search The text to look for
 * @return true if 'search' appears at the start of 'subject', false otherwise
 */
bool strBeginsWith (const std::string subject, const std::string search) {
  if (subject.length() < search.length())
    return false;

  return (boost::iequals(subject.substr(0, search.length()), search));
}

/**
 * Generic whitespace stream manipulator support functions
 */
Whitespace::Whitespace (int n) : num_spaces(n)
{
}

std::ostream& operator<<(std::ostream& stream, const Whitespace &ws)
{
  for(int i = 0; i < ws.num_spaces; i++)
    {
      stream << "  ";
    }
  return stream;
}

/**
 * Returns true if 'subject' is one of the characters in 'candidates'
 */
bool isOneOf (const char subject, const std::string &candidates)
{
  return (candidates.find(subject) != std::string::npos);
}

/**
 * Returns true if 'subject' contains only the characters in 'candidates'
 */
bool containsOnly (const std::string &subject, const std::string &candidates)
{
  for(size_t pos = 0; pos < subject.length(); pos++) {
    if (!isOneOf(subject[pos], candidates))
      return false;
  }
  return true;
}

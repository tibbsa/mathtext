/**
 * @file utility.cpp
 * Various miscellaneous utility functions that do not fit elsewhere.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <string>

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


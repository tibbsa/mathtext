/**
 * @file utility.h
 * Interface for miscellaneous utility functions.
 *
 * @copyright Copyright 2015 Anthony Tibbs.
 * This project is released under the GNU General Public License.
*/

#ifndef __UTILITY_H__
#define __UTILITY_H__

/**
 * Removes the extension (.XXX) from a filename, if there is one
 */
std::string remove_file_extension (const std::string &filename);

/**
 * Used primarily for logging, generic 'Whitespace' stream manipulator
 * Inspired by http://stackoverflow.com/questions/1391746/how-to-easily-indent-output-to-ofstream
 */
struct Whitespace {
  Whitespace(int n);
  int num_spaces;
};

std::ostream &operator<<(std::ostream &stream, const Whitespace &ws);

/**
 * Returns true if 'subject' begins with 'search' (case insensitive)
 */
bool strBeginsWith (const std::string subject, const std::string search);

/**
 * Returns true if 'subject' is one of the characters in 'candidates'
 */
bool isOneOf (const char subject, const std::string &candidates);

/**
 * Returns true if 'subject' contains only some combination of the characters
 * in 'candidates'.
 */
bool containsOnly (const std::string &subject, const std::string &candidates);

#endif /* __UTILITY_H__ */

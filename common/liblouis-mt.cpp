/**
 * @file liblouis-mt.cpp
 * Support functions for interfacing with the liblouis library
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"
#include "liblouis-mt.h"

void strToWideCharString(const std::string &str, ll_widechar *outString, unsigned &outLength)
{
  size_t inPos;
  unsigned outPos = 0;

  for (size_t inPos = 0; inPos < str.length(); inPos++)
    outString[outPos++] = (ll_widechar)str[inPos];

  outString[outPos] = 0;
}

void wideCharStringToStr(const ll_widechar *str, const unsigned inLength, std::string &outString)
{
  for (unsigned inPos = 0; inPos < inLength; inPos++)
    outString += std::string(1, (char)str[inPos]);
}


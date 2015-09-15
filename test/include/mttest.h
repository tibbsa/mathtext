/**
 * @file mttest.h
 * General header include for testing engine
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MTTEST_H__
#define __MTTEST_H__

#include "catch.hpp"
#include "MathDocument.h"

#define S(str) std::string(str)
#define CHARSTR(c) std::string(1, c)

void interpretToDocument (const std::string &input, MathDocument &document);
void documentToString (const MathDocument &document, std::string &outputString);

void checkInterpretation (const std::string &inputStr,
                          const std::string &expectedOutputStr);


#endif

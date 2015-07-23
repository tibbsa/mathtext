/**
 * @file mttest.cpp
 * Unit testing driver (to run on Linux)
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#define CATCH_CONFIG_MAIN
#include "mttest.h"

#include "MathDocumentElements.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"

void interpretToDocument (const std::string &input,
                          MathDocument &document,
                          std::string &outputString)
{
  CAPTURE(input);

  MathSourceFile src;
  REQUIRE_NOTHROW(src.loadFromBuffer (input));

  MathInterpreter interpreter(src, document);
  REQUIRE_NOTHROW(interpreter.interpret());
  REQUIRE(interpreter.haveMessages() == false);

  outputString.clear();
  BOOST_FOREACH (const MathDocumentElementPtr &ePtr, document.getDocument()) {
    const MathDocumentElement *ptr = ePtr.get();

    // Skip rendering of the original source line
    if(!dynamic_cast<const MDE_SourceLine *>(ptr))
      outputString += ptr->getString();
  }
}

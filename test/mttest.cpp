/**
 * @file mttest.cpp
 * Unit testing driver (to run on Linux)
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#define CATCH_CONFIG_MAIN
#include "mttest.h"
#include "MathDocumentElements.h"
#include "MathInterpreter.h"
#include "MathSourceFile.h"

void interpretToDocument (const std::string &input, MathDocument &document)
{
  CAPTURE(input);

  MathSourceFile src;
  REQUIRE_NOTHROW(src.loadFromBuffer (input));

  MathInterpreter interpreter(src, document);
  REQUIRE_NOTHROW(interpreter.interpret());
  REQUIRE(interpreter.haveMessages() == false);
}

void documentToString (const MathDocument &document, std::string &outputString)
{
  outputString.clear();
  BOOST_FOREACH (const MathDocumentElementPtr &ePtr, document.getDocument()) {
    const MathDocumentElement *ptr = ePtr.get();

    // Skip rendering of the original source line
    if(!dynamic_cast<const MDE_SourceLine *>(ptr))
      outputString += ptr->getString();
  }
}

void checkInterpretation (const std::string &inputStr,
			  const std::string &expectedOutputStr)
{
  MathDocument doc;
  std::string outputString;

  interpretToDocument (inputStr, doc);
  documentToString (doc, outputString);
  CHECK (outputString == (expectedOutputStr + "<eol>"));
}

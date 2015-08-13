/**
 * @file main.cpp
 * Command-line interface for MathText on Linux.
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include <boost/program_options.hpp>

#include "logging.h"
#include "utility.h"

#include "mathtext.h"
#include "MathExceptions.h"
#include "MathSourceFile.h"
#include "MathDocument.h"
#include "MathInterpreter.h"
#include "LaTeXRenderer.h"
#include "UEBRenderer.h"

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

/**
 * Parses the command line and launches the necessary conversions.
 *
 * @param argc Number of argments on command line
 * @param argv Array of c-strings containing command line arguments
 * @return An integer: 0 on success, 1 for translations errors, 2 for
 *         other errors
 */
int main (const int argc, const char **argv)
{
  string inputFilename;
  bool generateLaTeX = false;
  string latexOutputFilename;
  bool generateBraille = false;
  string brfOutputFilename;

  LOG_INFO << endl;
  LOG_INFO << "=====================================================";

  try {
    po::options_description desc("Allowed options");
    po::positional_options_description pod;
    po::variables_map vm;

    desc.add_options()
      ("help,h", "Display command line help")
      ("braille,b",
       po::value(&brfOutputFilename)->implicit_value (string()),
       "Generate a braille file (default: input name + .BRF)")

      ("latex,l",
       po::value(&latexOutputFilename)->implicit_value (string()),
       "Generate a LaTeX/print file (default: input name + .TEX)")

      ("file,f",
       po::value(&inputFilename)->required(),
       "Name of your input file/document")
      ;

    pod.add("file", -1);
    try {
      po::store(po::command_line_parser(argc, argv).
		options(desc).positional(pod).run(), vm);
      po::notify(vm);

      if (vm.count("help")) {
	cout << "Usage: mathtext [options] <filename>" << endl;
	cout << desc;
	return 2;
      }

      // If user did not choose either braille or print output then exit
      if (!vm.count("braille") && !vm.count("latex"))
	throw logic_error("No work to do -- must specify either or both of --braille and --latex");

      // If user did not provide filenames for braille/print output but
      // requested braille/print output, generate default filenames
      generateBraille = (vm.count("braille"));
      if (generateBraille && brfOutputFilename.empty())
	brfOutputFilename = remove_file_extension (inputFilename) + ".brf";

      generateLaTeX = (vm.count("latex"));
      if (generateLaTeX && latexOutputFilename.empty())
	latexOutputFilename = remove_file_extension (inputFilename) + ".tex";
    }
    catch(std::exception &e) {
      cerr << "Command line error: " << e.what() << endl << endl;
      cerr << "Usage: mathtext [options] <filename>" << endl;
      cerr << desc;
      return 2;
    }

    MathSourceFile srcfile;
    MathDocument doc;
    MathInterpreter interp(srcfile, doc);
    srcfile.loadFromFile(inputFilename);

    std::vector<std::string> renderCommands;
    LaTeXRenderer::getInterpreterCommandList (renderCommands);
    UEBRenderer::getInterpreterCommandList (renderCommands);
    interp.registerCommands (renderCommands);

    try {
      interp.interpret();
    } catch (MathInterpreterException &e) {
      cout << "An error occurred and translation of your document was stopped." << endl;
      if (!interp.haveMessages()) {
	cout << "Cause of execption unknown: no messages produced!" << endl;
      } else {
	const std::vector<MathInterpreterMsg> &msgs = interp.getMessages();
	cout << msgs.size() << " message(s):" << endl;

	for (std::vector<MathInterpreterMsg>::const_iterator it = msgs.begin();
	     it != msgs.end();
	     ++it) {
	  cout << "- " << *it << endl;
	}
      }

      return 1;
    }

    if (interp.haveMessages()) {
      const std::vector<MathInterpreterMsg> &msgs = interp.getMessages();
      cout << msgs.size() << " message(s):" << endl;

      for (std::vector<MathInterpreterMsg>::const_iterator it = msgs.begin();
	   it != msgs.end();
	   ++it) {
	cout << "- " << *it << endl;
      }
    }

    if (generateLaTeX) {
      LOG_INFO << endl << "====================== LaTeX Render ===================" << endl;

      LaTeXRenderer ltr;
      std::string output;
      output = ltr.renderDocument(doc);

      LOG_INFO << endl << "^^^^^^^^^^^^^^^^^^^^^^^^^^ LATEX ^^^^^^^^^^^^^^^^^^^^^^" << endl;

      LOG_INFO << output;
      LOG_INFO << endl << "vvvvvvvvvvvvvvvvvvvvvvvvvv LATEX vvvvvvvvvvvvvvvvvvvvvv" << endl;

      ofstream ofs;
      ofs.open(latexOutputFilename.c_str());
      if (!ofs.is_open()) {
	cerr << "error: unable to open '" << latexOutputFilename << "' for writing!" << endl;
	return 2;
      }

      ofs << output;
      if (!ofs.good()) {
	cerr << "error occurred while writing to '" << latexOutputFilename << "'!" << endl;
	ofs.close();
	return 2;
      }

      ofs.close();
    }

    if (generateBraille) {
      LOG_INFO << endl << "====================== UEB Render ===================" << endl;

      std::string output;
      UEBRenderer ueb;
      ueb.enableLineWrapping (30);
      output = ueb.renderDocument(doc);


      LOG_INFO << endl << "^^^^^^^^^^^^^^^^^^^^^^^^^^  UEB  ^^^^^^^^^^^^^^^^^^^^^^" << endl;

      LOG_INFO << output;
      LOG_INFO << endl << "vvvvvvvvvvvvvvvvvvvvvvvvvv  UEB  vvvvvvvvvvvvvvvvvvvvvv" << endl;

      ofstream ofs;
      ofs.open(brfOutputFilename.c_str());
      if (!ofs.is_open()) {
	cerr << "error: unable to open '" << brfOutputFilename << "' for writing!" << endl;
	return 2;
      }

      ofs << output;
      if (!ofs.good()) {
	cerr << "error occurred while writing to '" << brfOutputFilename << "'!" << endl;
	ofs.close();
	return 2;
      }

      ofs.close();
    }
  }
  catch (MathDocumentException &e) {
    logResetIndent();

    std::string const *error = boost::get_error_info<mdx_error_info>(e);
    std::string const *file = boost::get_error_info<mdx_filename_info>(e);
    std::string const *desc = boost::get_error_info<mdx_liberrmsg_info>(e);

    LOG_FATAL << "=======================================" << endl;
    LOG_FATAL << boost::diagnostic_information(e);
    LOG_FATAL << "=======================================" << endl;

    if (error)
      cerr << *error;
    else
      cerr << "Unspecified error ";

    if (file)
      cerr << " [" << *file << "]";
    if (desc)
      cerr << " - " << *desc;
    cerr << endl;
    return 2;
  }
  catch (boost::exception &e) {
    LOG_FATAL << "Uncaught exception in main(): " << boost::diagnostic_information(e);

    cerr << "terminating on unhandled exception: " << boost::diagnostic_information(e) << endl;
    return 2;
  }


  return 0;
}

/**
 * @file main.cpp
 * Command-line interface for MathText on Linux.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <iostream>
#include <boost/program_options.hpp>

#include "mathtext.h"
#include "utility.h"

using namespace std;
namespace po = boost::program_options;

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
  try {
    string inputFilename;
    bool generateLaTeX = false;
    string latexOutputFilename;
    bool generateBraille = false;
    string brfOutputFilename;

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
      cout << "Usage: mathtext [options] <filename>" << endl;
      cout << desc;
      return 2;
    }

    MathTranslator mt;

    try {
      mt.loadFromFile(inputFilename);
    }
    catch (std::exception &e) {
      cerr << "terminating on unhandled translation exception: " << e.what() << endl << endl;
      return 2;
    }
  }
  catch (std::exception &e) {
    cerr << "terminating on unhandled exception: " << e.what() << endl << endl;
    return 2;
  }

  return 0;
}

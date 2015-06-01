/**
 * @file main.cpp
 * Command-line interface for MathText on Linux.
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

using namespace std;

#include <iostream>

#include "mathtext.h"

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
  cout << "MathText " << MATHTEXT_VERSION_NUMBER ", build " <<
    MATHTEXT_VERSION_BUILD << " - " << MATHTEXT_VERSION_TAGLINE << endl;
  cout << "Copyright (C) 2015 Anthony Tibbs" << endl << endl;
  cout << "This program is FREE SOFTWARE and comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "See LICNESE for more information." << endl << endl;

  return 0;
}

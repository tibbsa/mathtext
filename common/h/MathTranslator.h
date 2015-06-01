/**
 * @file MathTranslator.h
 * Header file for the main math translator class
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#ifndef __MATH_TRANSLATOR_H__
#define __MATH_TRANSLATOR__H__

/**
 * The main operating logic of the translation engine.
 */
class MathTranslator
{
 public:
  void loadFromFile (const std::string &filename);
};

#endif /* __MATH_TRANSLATOR_H__ */

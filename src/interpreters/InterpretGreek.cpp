/**
 * @file InterpretGreek.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/


#include "common-includes.h"
#include "logging.h"

#include "MathInterpreter.h"

/**
 * Attempts to interpret a greek letter: %a, %b, etc.
 *
 * Returns true on success, false on error, and puts resulting elements
 * into the 'target' buffer.
 */
namespace {
  struct GreekLetterMap {
    GreekLetterMap (const char s, const MDE_GreekLetter::Character c) : symbol(s), character(c) {}
    char symbol;
    MDE_GreekLetter::Character character;
  };
}

bool MathInterpreter::interpretGreekLetter (MDEVector &target,
					 const std::string &src,
					 size_t &i)
{

#define CHARMAP(symbol1,char1,symbol2,char2) ( GreekLetterMap(symbol1, MDE_GreekLetter::char1) ) ( GreekLetterMap(symbol2, MDE_GreekLetter::char2) )
  static const std::vector<GreekLetterMap> map = boost::assign::list_of
    CHARMAP('a', alpha, 'A', Alpha)
    CHARMAP('b', beta, 'B', Beta)
    CHARMAP('g', gamma, 'G', Gamma)
    CHARMAP('d', delta, 'D', Delta)
    CHARMAP('e', epsilon, 'E', Epsilon)
    CHARMAP('z', zeta, 'Z', Zeta)
    CHARMAP('h', eta, 'H', Eta)
    CHARMAP('t', theta, 'T', Theta)
    CHARMAP('i', iota, 'I', Iota)
    CHARMAP('k', kappa, 'K', Kappa)
    CHARMAP('l', lambda, 'L', Lambda)
    CHARMAP('m', mu, 'M', Mu)
    CHARMAP('n', nu, 'N', Nu)
    CHARMAP('x', xi, 'X', Xi)
    CHARMAP('o', omicron, 'O', Omicron)
    CHARMAP('p', pi, 'P', Pi)
    CHARMAP('q', tau, 'Q', Tau)
    CHARMAP('r', rho, 'R', Rho)
    CHARMAP('s', sigma, 'S', Sigma)
    CHARMAP('u', upsilon, 'U', Upsilon)
    CHARMAP('v', phi, 'V', Phi)
    CHARMAP('c', chi, 'C', Chi)
    CHARMAP('f', psi, 'F', Psi)
    CHARMAP('w', omega, 'W', Omega)
    ;

  if (src [i] != '%' || (i+1) >= src.length())
    return false;

  char c = src [i+1];
  if (c == '%') /* %% is the symbol for 'percent sign' */
    return false;

  for (std::vector<GreekLetterMap>::const_iterator it = map.begin();
       it != map.end(); ++it) {
    const GreekLetterMap &mi = *it;
    // Look for the first character
    if (c == mi.symbol) {
      LOG_TRACE << "* added greek letter for (" << mi.symbol << ")";
      target.push_back (boost::make_shared<MDE_GreekLetter>(mi.character));
      i += 2;

      return true;
    }
  }

  MSG_WARNING(UNKNOWN_GREEK, boost::str(boost::format("'%%%c' does not represent a greek letter") % c));
  return false;
}

/**
 * @file MDE_GreekLetter.cpp
 * Greek letters
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include <boost/assign.hpp>
#include <map>
#include "MathDocumentElements.h"

/* ========================= PUBLIC FUNCTION =============================== */
MDE_GreekLetter::MDE_GreekLetter(const MDE_GreekLetter::Character c) : letter(c)
{
}

MDE_GreekLetter::Character MDE_GreekLetter::getValue (void) const
{
  return letter;
}

void MDE_GreekLetter::setValue (const MDE_GreekLetter::Character c)
{
  letter = c;
}

std::string MDE_GreekLetter::getName (void) const
{
#define CHARMAP(a,b,c,d) (a, #a) (b, #b) (c, #c) (d, #d)
#define CHARMAP2(a,b) (a, #a) (b, #b)

  static std::map<MDE_GreekLetter::Character,std::string> charmap = boost::assign::map_list_of
    CHARMAP(alpha, Alpha, beta, Beta)
    CHARMAP(gamma, Gamma, delta, Delta)
    CHARMAP(epsilon, Epsilon, zeta, Zeta)
    CHARMAP2(eta, Eta)
    CHARMAP(theta, Theta, iota, Iota)
    CHARMAP(kappa, Kappa, lambda, Lambda)
    CHARMAP(mu, Mu, nu, Nu)
    CHARMAP(xi, Xi, omicron, Omicron)
    CHARMAP(pi, Pi, rho, Rho)
    CHARMAP(sigma, Sigma, tau, Tau)
    CHARMAP(upsilon, Upsilon, phi, Phi)
    CHARMAP(chi, Chi, psi, Psi)
    CHARMAP2(omega, Omega)
    ;

  return charmap [letter];
}

/**
 * Generate a string representation of this element (primarily for
 * debugging purposes)
 *
 * @return string representation of this element
 */
std::string MDE_GreekLetter::getString (void) const
{
  return std::string("<GREEK:" + getName() + ">");
}



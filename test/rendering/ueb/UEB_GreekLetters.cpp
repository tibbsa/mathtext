/**
 * @file Comparators.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

#define R(ltr,brl) { MDE_GreekLetter l(MDE_GreekLetter::ltr); CHECK(r.renderGreekLetter(&l) == brl); }

TEST_CASE("render/ueb/GreekLetters", "[render][UEB][GreekLetter]") {
  UEBRenderer r;

  // UEB Rulebook 11.7 ---------------------------------------------------
  R(alpha, ".A");
  R(Alpha, ",.A");
  R(beta, ".B");
  R(Beta, ",.B");
  R(gamma, ".G");
  R(Gamma, ",.G");
  R(delta, ".D");
  R(Delta, ",.D");
  R(epsilon, ".E");
  R(Epsilon, ",.E");
  R(zeta, ".Z");
  R(Zeta, ",.Z");
  R(eta, ".:");
  R(Eta, ",.:");
  R(theta, ".?");
  R(Theta, ",.?");
  R(iota, ".I");
  R(Iota, ",.I");
  R(kappa, ".K");
  R(Kappa, ",.K");
  R(lambda, ".L");
  R(Lambda, ",.L");
  R(mu, ".M");
  R(Mu, ",.M");
  R(nu, ".N");
  R(Nu, ",.N");
  R(xi, ".X");
  R(Xi, ",.X");
  R(omicron, ".O");
  R(Omicron, ",.O");
  R(pi, ".P");
  R(Pi, ",.P");
  R(rho, ".R");
  R(Rho, ",.R");
  R(sigma, ".S");
  R(Sigma, ",.S");
  R(tau, ".T");
  R(Tau, ",.T");
  R(upsilon, ".U");
  R(Upsilon, ",.U");
  R(phi, ".F");
  R(Phi, ",.F");
  R(chi, ".&");
  R(Chi, ",.&");
  R(psi, ".Y");
  R(Psi, ",.Y");
  R(omega, ".W");
  R(Omega, ",.W");
}




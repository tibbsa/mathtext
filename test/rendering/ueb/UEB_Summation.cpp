/**
 * @file UEB_Summation.cpp
 * 
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

TEST_CASE("render/ueb/Summations/nolimits", "[render][UEB][Summations]") {
  UEBRenderer r;

  // UEB Technical Guideline 7.9 --------------------------------------------
  SECTION ("Summation with no limits") {
    MDEVector lower_bound, upper_bound;

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S");
  }
}

TEST_CASE("render/ueb/Summations/lower_limits", "[render][UEB][Summations]") {
  UEBRenderer r;

  SECTION ("Summation with simple, numeric lower limit") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.5#D");
  }

  SECTION ("Summation with simple, numeric lower limit followed by letter-indicator requiring letter") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

    MDEVector vec;
    vec.push_back (boost::make_shared<MDE_Summation>(lower_bound, upper_bound));
    vec.push_back (boost::make_shared<MDE_MathBlock>("a"));

    CHECK (r.renderVector (vec) == ",.S.5#D;a");
  }

  SECTION ("Summation with simple, alphabetic lower limit") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_MathBlock>("n"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.5n");
  }

  SECTION ("Summation with complex lower limit") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));
    lower_bound.push_back (boost::make_shared<MDE_MathBlock>("a"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.5<#D;a>");
  }
}

TEST_CASE("render/ueb/Summations/upper_limits", "[render][UEB][Summations]") {
  UEBRenderer r;

  SECTION ("Summation with simple, numeric upper limit") {
    MDEVector lower_bound, upper_bound;

    upper_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.9#D");
  }

  SECTION ("Summation with simple, numeric upper limit followed by letter-indicator requiring letter") {
    MDEVector lower_bound, upper_bound;

    upper_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));

    MDEVector vec;
    vec.push_back (boost::make_shared<MDE_Summation>(lower_bound, upper_bound));
    vec.push_back (boost::make_shared<MDE_MathBlock>("a"));

    CHECK (r.renderVector (vec) == ",.S.9#D;a");
  }

  SECTION ("Summation with simple, alphabetic upper limit") {
    MDEVector lower_bound, upper_bound;

    upper_bound.push_back (boost::make_shared<MDE_MathBlock>("n"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.9n");
  }

  SECTION ("Summation with complex upper limit") {
    MDEVector lower_bound, upper_bound;

    upper_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));
    upper_bound.push_back (boost::make_shared<MDE_MathBlock>("a"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.9<#D;a>");
  }
}

TEST_CASE("render/ueb/Summations/complex", "[render][UEB][Summations]") {
  UEBRenderer r;

  // Test to make sure we don't get a letter indicator in the upper limit 
  // when it is followed by a lower limit containing a number.
  SECTION ("Summation with simple, numeric lower limit and alphabetic upper limit") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));
    upper_bound.push_back (boost::make_shared<MDE_MathBlock>("a"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.5#D.9a");
  }

  SECTION ("Summation with complex lower and upper limits") {
    MDEVector lower_bound, upper_bound;

    lower_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "4", ""));
    lower_bound.push_back (boost::make_shared<MDE_MathBlock>("a"));

    upper_bound.push_back (boost::make_shared<MDE_Number>(MDE_Number::NEGATIVE, "4", ""));
    upper_bound.push_back (boost::make_shared<MDE_MathBlock>("x"));

    const MDE_Summation sum(lower_bound, upper_bound);
    CHECK (r.renderSummation (&sum) == ",.S.5<#D;a>.9<-#Dx>");
  }
}



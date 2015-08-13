/**
 * @file UEB_Modifiers.cpp
 *
 * @copyright Copyright 2015 Anthony Tibbs
 * This project is released under the GNU General Public License.
*/

#include "common-includes.h"

#include "mttest.h"
#include "MathDocumentElements.h"
#include "UEBRenderer.h"

// Modifiers can appear with or without grouping indicators.  Grouping
// indicators are not required for braille items that are considered
// "items".  We're not interested here in testing whether item detection
// works properly for all test cases -- only that when items appear,
// grouping indicators are ignored.  Item detection tests are separate, under
// 'render/ueb/item_detection'.  Here, we use a single number (2) to serve
// as a simple "item", and a negative number (-2) to serve as an item
// requiring grouping indicators (since the minus sign can itself be an
// item).

namespace {
  struct ModifierTest {
    ModifierTest (const MDE_Modifier::Modifier mod,
		  const std::string &brlUngrouped,
		  const std::string &brlGrouped) : modifier(mod), ungrouped(brlUngrouped), grouped(brlGrouped) {}

    MDE_Modifier::Modifier modifier;
    std::string ungrouped;
    std::string grouped;
  };
}

TEST_CASE("render/ueb/Modifiers", "[render][UEB][Modifiers]") {
  UEBRenderer r;

  const std::vector<ModifierTest> tests = boost::assign::list_of
    ( ModifierTest (MDE_Modifier::OVER_BAR, "#B:", "<-#B>:") )
    ( ModifierTest (MDE_Modifier::OVER_ARROW_RIGHT, "#B^:", "<-#B>^:") )
    ( ModifierTest (MDE_Modifier::OVER_HAT, "#B\":", "<-#B>\":") )
    ;

  MDEVector positive_number_vector, negative_number_vector;
  positive_number_vector.push_back (boost::make_shared<MDE_Number>(MDE_Number::POSITIVE, "2", ""));
  negative_number_vector.push_back (boost::make_shared<MDE_Number>(MDE_Number::NEGATIVE, "2", ""));

  // UEB Technical Guidelines, Section 12 ----------------------------------

  BOOST_FOREACH ( const ModifierTest &curTest, tests ) {
    std::string sectionName = boost::str(boost::format("modifier: %s") % MDE_Modifier::getModifierName(curTest.modifier));
    SECTION ( sectionName ) {
      MDE_Modifier ungrouped_modifier (curTest.modifier, positive_number_vector);
      CHECK (r.renderModifier(&ungrouped_modifier) == curTest.ungrouped);


      MDE_Modifier grouped_modifier (curTest.modifier, negative_number_vector);
      CHECK (r.renderModifier(&grouped_modifier) == curTest.grouped);
    }
  }
}




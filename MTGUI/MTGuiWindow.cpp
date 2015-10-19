/**
* @file
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#include "MTGuiWindow.h"

/**
 * Set up the main GUI window
 */
MTGUIWindow::MTGUIWindow(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);
}

MTGUIWindow::~MTGUIWindow()
{

}

/**
 * Called when the user selects File -> Exit to close the application.
 */
void MTGUIWindow::on_exit(void)
{
  this->close();
}

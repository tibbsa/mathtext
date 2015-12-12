/**
* @file
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#include "MTGuiWindow.h"
#include "MTDocWindow.h"

/**
 * Set up the main GUI window
 */
MTGUIWindow::MTGUIWindow(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);
  ui.mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui.mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
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

/**
 * Called when the user selects File -> New to create a new, blank file.
 */
void MTGUIWindow::on_file_new(void)
{
  MTDocWindow *doc = createDocumentWindow();
  doc->newFile();
  doc->show();
}


/**
 * Sets up a new document window.  Used when creating new documents or 
 * performing initial setup to load existing documents.
 *
 * @return MTDocumentWindow* Pointer to a newly created and setup doc window.
 */
MTDocWindow *MTGUIWindow::createDocumentWindow(void)
{
  MTDocWindow *newWin = new MTDocWindow();
  ui.mdiArea->addSubWindow(newWin);

  /***************
    connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
  */

  return newWin;
}
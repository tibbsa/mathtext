/**
* @file
*
* Implementation of the MTDocWindow class.
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#include <QtWidgets>
#include "MTDocWindow.h"

MTDocWindow::MTDocWindow()
{
  setAttribute(Qt::WA_DeleteOnClose);
}

void MTDocWindow::newFile(void)
{
  /**
   * Used to auto-generate "Untitled#" file names when new files are opened.
   */
  static int newFileSequenceNumber = 1; 

  isUntitled = true;
  documentFilename = tr("Untitled%1.mtx").arg(newFileSequenceNumber++);
  setWindowTitle(documentFilename + "[*]");
}


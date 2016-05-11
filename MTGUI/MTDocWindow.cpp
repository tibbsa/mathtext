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

bool MTDocWindow::loadFile(const QString &filename)
{
  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("MathText"),
      tr("Unable to load file %1:\n%2.")
      .arg(filename)
      .arg(file.errorString()));
    return false;
  }

  QTextStream streamer(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  setPlainText(streamer.readAll());
  QApplication::restoreOverrideCursor();

  connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

  documentFilename = QFileInfo(filename).canonicalFilePath();
  isUntitled = false;
  document()->setModified(false);
  setWindowModified(false);
  setWindowTitle(getFileTitle() + "[*]");

  return true;
}

bool MTDocWindow::save(void)
{
  if (isUntitled)
    return saveAs();
  
  QFile file(documentFilename);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("MDI"),
      tr("Cannot write file %1:\n%2.")
      .arg(documentFilename)
      .arg(file.errorString()));
    return false;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  out << toPlainText();
  QApplication::restoreOverrideCursor();

  documentFilename = QFileInfo(documentFilename).canonicalFilePath();
  isUntitled = false;
  document()->setModified(false);
  setWindowModified(false);
  setWindowTitle(getFileTitle() + "[*]");

  return true;
}

bool MTDocWindow::saveAs(void)
{
  QString filename =
    QFileDialog::getSaveFileName(this, tr("Save MathText Document"),
    documentFilename, tr("MathText Documents (*.mtx);;Text Files (*.txt);;All Files (*.*)"));
  
  if (filename.isEmpty())
    return false;

  documentFilename = QFileInfo(documentFilename).canonicalFilePath();
  isUntitled = false;
  return save();
}

void MTDocWindow::closeEvent(QCloseEvent *event)
{
  if (querySaveOnClose())
    event->accept();
  else
    event->ignore();
}

void MTDocWindow::documentWasModified(void)
{
  setWindowModified(document()->isModified());
}

/**
 * Returns the basic name of the current file (no path or extension)
 *
 * @returns QString containing name of file (no path or extension)
 */
QString MTDocWindow::getFileTitle(void)
{
  return QFileInfo(documentFilename).fileName();
}

bool MTDocWindow::querySaveOnClose(void)
{
  if (document()->isModified()) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("MathText"),
      tr("'%1' has been modified.\n"
      "Do you want to save your changes?")
      .arg(getFileTitle()),
      QMessageBox::Save | QMessageBox::Discard
      | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }

  return true;
}
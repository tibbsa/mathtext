/**
* @file
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#include <QtWidgets>

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

  connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
    this, SLOT(updateMenus()));

  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

  updateMenus();
}

MTGUIWindow::~MTGUIWindow()
{

}

void MTGUIWindow::closeEvent(QCloseEvent *event)
{
  ui.mdiArea->closeAllSubWindows();
  /* if closing did not work and we still have an active window, do not exit */
  if (ui.mdiArea->currentSubWindow()) {
    event->ignore();  
  } else {
    event->accept();
  }
}

/**
 * Called when the user selects File -> Exit to close the application.
 */
void MTGUIWindow::cmd_exit(void)
{
  this->close();
}

/**
 * Called when the user selects File -> New to create a new, blank file.
 */
void MTGUIWindow::cmd_file_new(void)
{
  MTDocWindow *doc = createDocumentWindow();
  doc->newFile();
  doc->show();
}

void MTGUIWindow::cmd_file_open(void)
{
  QString filename =
    QFileDialog::getOpenFileName(this, tr("Open MathText Document"),
                                 QString(), tr("MathText Documents (*.mtx);;Text Files (*.txt);;All Files (*.*)"));
  if (!filename.isEmpty()) {
    MTDocWindow *doc = createDocumentWindow();
    if (doc->loadFile(filename))
      doc->show();
    else
      doc->close();
  }
}

void MTGUIWindow::cmd_file_save(void)
{
  if (activeDocumentWindow())
    activeDocumentWindow()->save();
}

void MTGUIWindow::cmd_file_saveAs(void)
{
  if (activeDocumentWindow())
    activeDocumentWindow()->saveAs();
}

void MTGUIWindow::cmd_file_close(void)
{
  ui.mdiArea->closeActiveSubWindow();
}

void MTGUIWindow::cmd_edit_cut(void)
{
  if (activeDocumentWindow())
    activeDocumentWindow()->cut();
}

void MTGUIWindow::cmd_edit_copy(void)
{
  if (activeDocumentWindow())
    activeDocumentWindow()->copy();
}

void MTGUIWindow::cmd_edit_paste(void)
{
  if (activeDocumentWindow())
    activeDocumentWindow()->paste();
}

/**
 * Called from time-to-time to make sure that the enabled menu items 
 * match the current state of the application (e.g. you can't 'save' if 
 * no files are opened.
 */
void MTGUIWindow::updateMenus(void)
{
  MTDocWindow *activeDoc = activeDocumentWindow();

  bool isDocOpen = (activeDoc != NULL);
  ui.action_File_Save->setEnabled(isDocOpen);
  ui.action_File_SaveAs->setEnabled(isDocOpen);
  ui.action_File_Close->setEnabled(isDocOpen);
  
  bool isTextSelected = isDocOpen && activeDoc->textCursor().hasSelection();
  ui.action_Edit_Copy->setEnabled(isTextSelected);
  ui.action_Edit_Cut->setEnabled(isTextSelected);
  ui.action_Edit_Paste->setEnabled(isDocOpen);
  
  updateWindowMenu();
}

/**
 * Called to update the 'Window' menu.
 */
class _ActiveDocWindowSubwindowFunctor {
public:
  explicit _ActiveDocWindowSubwindowFunctor(QMdiArea *mdiArea, QMdiSubWindow *activeWindow) : m_mdiArea(mdiArea), m_activeWindow(activeWindow) {}
  void operator()() const { m_mdiArea->setActiveSubWindow(m_activeWindow); }

private:
  QMdiArea *m_mdiArea;
  QMdiSubWindow *m_activeWindow;
};

/**
 * Refreshes the 'window' menu with active/open files
 */
void MTGUIWindow::updateWindowMenu(void)
{
  ui.menu_Window->clear();
  QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
  for (int i = 0; i < windows.size(); ++i) {
    QMdiSubWindow *mdiSubWindow = windows.at(i);
    MTDocWindow *child = qobject_cast<MTDocWindow *>(mdiSubWindow->widget());

    QString text;
    if (i < 9) {
      text = tr("&%1 %2").arg(i + 1)
                         .arg(child->getFileTitle());
      ;
    }
    else {
      text = tr("%1 %2").arg(i + 1)
                        .arg(child->getFileTitle());
    }
    QAction *action = ui.menu_Window->addAction(text);
    action->setCheckable(true);
    action->setChecked(child == activeDocumentWindow());
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, windows.at(i));
  }
}

/**
 * Called when a new subwindow is triggered, to pass on the activation 
 * message to our MDI area handler.
 */
void MTGUIWindow::setActiveSubWindow(QWidget *window)
{
  if (!window)
    return;

  ui.mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
  updateMenus();
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

  connect(newWin, SIGNAL(copyAvailable(bool)), ui.action_Edit_Cut, SLOT(setEnabled(bool)));
  connect(newWin, SIGNAL(copyAvailable(bool)), ui.action_Edit_Copy, SLOT(setEnabled(bool)));
  
  return newWin;
}

/**
 * Returns the active document window, if there is one.  NULL otherwise.
 *
 * @return MTDocumentWindow* Pointer to the currently active document window.
 */
MTDocWindow *MTGUIWindow::activeDocumentWindow(void)
{
  if (QMdiSubWindow *activeSubWindow = ui.mdiArea->activeSubWindow())
    return qobject_cast<MTDocWindow *>(activeSubWindow->widget());

  return NULL;
}


/**
* @file
*
* @copyright Copyright 2015 Anthony Tibbs
* This project is released under the GNU General Public License.
*/
#ifndef MTGUIWINDOW_H
#define MTGUIWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MTGuiWindow.h"

QT_BEGIN_NAMESPACE
class MTDocWindow;
class QMdiArea;
class QSignalMapper;
QT_END_NAMESPACE

class MTGUIWindow : public QMainWindow
{
  Q_OBJECT

public:
  MTGUIWindow(QWidget *parent = 0);
  ~MTGUIWindow();

protected:
  void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
  void cmd_exit(void);
  void cmd_file_new(void);
  void cmd_file_open(void);
  void cmd_file_save(void);
  void cmd_file_saveAs(void);
  void cmd_file_close(void);
  void cmd_edit_cut(void);
  void cmd_edit_copy(void);
  void cmd_edit_paste(void);
  
  void updateMenus(void);
  void updateWindowMenu(void);
  
  void setActiveSubWindow(QWidget *window);

private:
  Ui::MTGUIWindowClass ui;
  QSignalMapper *windowMapper;

  MTDocWindow *createDocumentWindow(void);
  MTDocWindow *activeDocumentWindow(void);
};

#endif // MTGUIWINDOW_H

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

class MTDocWindow;
class QMdiArea;

class MTGUIWindow : public QMainWindow
{
  Q_OBJECT

public:
  MTGUIWindow(QWidget *parent = 0);
  ~MTGUIWindow();

private slots:
  void on_exit(void);
  void on_file_new(void);

private:
  Ui::MTGUIWindowClass ui;

  MTDocWindow *createDocumentWindow(void);
};

#endif // MTGUIWINDOW_H

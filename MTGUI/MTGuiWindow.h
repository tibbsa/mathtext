#ifndef MTGUIWINDOW_H
#define MTGUIWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MTGuiWindow.h"

class MTGUIWindow : public QMainWindow
{
    Q_OBJECT

public:
    MTGUIWindow(QWidget *parent = 0);
    ~MTGUIWindow();

private:
    Ui::MTGUIWindowClass ui;
};

#endif // MTGUIWINDOW_H

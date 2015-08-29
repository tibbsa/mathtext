#ifndef CMATHTEXTWINDOW_H
#define CMATHTEXTWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_CMathTextWindow.h"

class MathDocument;

class CMathTextWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMathTextWindow(QWidget *parent = 0);
    ~CMathTextWindow();

private slots:
    void on_actionFileExit_triggered(void);
    void on_actionFileConvertToBraille_triggered(void);
    void on_actionFileConvertToLaTeX_triggered(void);
    void on_actionFileConvertToPDF_triggered(void);

private:
    Ui::CMathTextWindowClass ui;
    bool loadFile(const QString &inputFilename, MathDocument &doc);
};

#endif // CMATHTEXTWINDOW_H

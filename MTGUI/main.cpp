#include "MTGuiWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MTGUIWindow w;
    w.show();
    return a.exec();
}

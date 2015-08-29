#include "CMathTextWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMathTextWindow w;
    w.show();
    return a.exec();
}

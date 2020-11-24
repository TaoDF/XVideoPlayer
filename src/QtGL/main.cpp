#include "QtGL.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtGL w;
    w.show();
    return a.exec();
}

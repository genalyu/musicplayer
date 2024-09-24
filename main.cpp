#include "mianwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MianWidget w;
    w.show();
    return a.exec();
}

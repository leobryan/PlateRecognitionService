#include <QApplication>
#include "service.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Service w;
    w.show();

    return a.exec();
}

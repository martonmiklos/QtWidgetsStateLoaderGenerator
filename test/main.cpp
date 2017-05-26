#include <QApplication>
#include <QDebug>

#include "formtest2.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MM");
    QCoreApplication::setApplicationName("QtWidgetsStateLoaderGeneratorTest");
    QCoreApplication::setApplicationVersion("1.0");

    QApplication a(argc, argv);

    FormTest2 w;
    w.show();
    return a.exec();
}

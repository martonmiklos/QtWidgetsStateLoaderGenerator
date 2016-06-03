#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

#include "mainwindow.h"
#include "projectparser.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("MM");
    QCoreApplication::setApplicationName("QtWidgetsStateLoaderGenerator");
    QCoreApplication::setApplicationVersion("1.0");

    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();

    // An option with a value
    QCommandLineOption projectOption(QStringList() << "p" << "project",
            QCoreApplication::translate("main", "Regenerates the code accoding to the project file."),
            QCoreApplication::translate("main", "projectfile"));
    parser.addOption(projectOption);


    // Process the actual command line arguments given by the user
    parser.process(a);
    QString projectPath = parser.value(projectOption);

    if (projectPath.isEmpty()) {
        MainWindow w;
        w.show();
        return a.exec();
    } else {
        ProjectParser p;
        p.parseProject(projectPath);
        if (p.generate()) {
            qDebug() << QCoreApplication::translate("main", "Code generation was successful");
            return 0;
        } else {
            qDebug() << QCoreApplication::translate("main", "Code generation failed: %1").arg(p.error());
            return -1;
        }
    }
}

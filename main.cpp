#include <QApplication>
#include "tasklist_main.h"
#include <QFile>

int main(int argc, char *argv[])
{
    //Setup style sheets
    QApplication app(argc, argv);
    QFile File(":style/all/style.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    //Initialize resources
    Q_INIT_RESOURCE(images);
    TaskList_Main *myDialog = new TaskList_Main;
    myDialog->show();

    //Apply style sheets
    qApp->setStyleSheet(StyleSheet);
    return app.exec();
}


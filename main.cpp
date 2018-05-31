#include "CbrMainWindow.h"
#include <QApplication>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString qss;
    QFile qssFile(":/qrc/style.qss");
    qssFile.open(QFile::ReadOnly);

    if(qssFile.isOpen())
    {
        qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }
    CbrMainWindow w;
    w.setWindowTitle("ComicsBookReader");
//    w.show();
//    w.showFullScreen();
    w.showMaximized();
//    qApp->processEvents();

    return a.exec();
}

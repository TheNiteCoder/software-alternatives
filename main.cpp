#include "window.h"
#include <QApplication>
#include <QTranslator>

#include "unistd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTran;
    qtTran.load(QString("qt_") + QLocale::system().name());
    a.installTranslator(&qtTran);

    QTranslator appTran;
    appTran.load(QString("software-alternatives_") + QLocale::system().name(), "/usr/share/software-alternatives/locale");
    a.installTranslator(&appTran);

    if (getuid() == 0) {
        Window w;
        w.show();

        return a.exec();
    } else {
#ifdef QT_DEBUG
        system("su-to-root -X -c " + QCoreApplication::applicationFilePath().toUtf8() + "");
#else
        system("su-to-root -X -c " + QCoreApplication::applicationFilePath().toUtf8() + "&");
#endif
    }
}

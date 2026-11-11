#include <QApplication>
#include <QSettings>
#include <QDebug>

#include "mainui.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


    MainUI w;
    w.resize(800,600);
    w.show();

    return app.exec();
}








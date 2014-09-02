#include "mainwindow.h"
#include <QApplication>
#include <thread>
#include <iostream>
#include <stdio.h>
#include "SLogic.h"

int main(int argc, char *argv[])
{
    // starting point
    QApplication application(argc, argv);
    MainWindow w;
    w.show();
    SLogic::GetInstance();
    return application.exec();
}



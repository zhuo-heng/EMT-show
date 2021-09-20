/******************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
//
// Version  : 2.0
// Date     : Mon Aug 29 19:40:09 2021
// Author   : hengzhuo
// Note     : 本程序基于Qt5实现
/******************************************************************************************/
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/******************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.

// Time : 2021/08/27
// master : hengzhuo
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

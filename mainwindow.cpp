/***********************************************************************************************/
// function : mainwindow.cpp
// 
// 负责初始化主窗口所有的按钮、标签
// VTK的渲染显示
// 
//
/***********************************************************************************************/
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);




}

MainWindow::~MainWindow()
{
    delete ui;
}

//开始追踪
void MainWindow::StartTrack()
{

}

//保存追踪数据
void MainWindow::OnSave()
{

}

//退出UI界面
void MainWindow::exitUI()
{

}

//渲染图视角复位
void MainWindow::vtkViewReset()
{

}
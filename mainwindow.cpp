/***********************************************************************************************/
// function : mainwindow.cpp
// 
// �����ʼ�����������еİ�ť����ǩ
// VTK����Ⱦ��ʾ
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

//��ʼ׷��
void MainWindow::StartTrack()
{

}

//����׷������
void MainWindow::OnSave()
{

}

//�˳�UI����
void MainWindow::exitUI()
{

}

//��Ⱦͼ�ӽǸ�λ
void MainWindow::vtkViewReset()
{

}
/***********************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
// 
// function : mainwindow.cpp
// 
// 负责初始化主窗口所有的按钮、标签
// VTK的渲染显示
// 渲染模型的初始化
/***********************************************************************************************/
#include "mainwindow.h"
#include "./ui_mainwindow.h"


#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EMT-show");
    //this->setWindowFlags(Qt::FramelessWindowHint);//去除顶部标题框

    m_polydata        =     vtkSmartPointer<vtkPolyData>::New();
    m_recordPolydata  =     vtkSmartPointer<vtkPolyData>::New();
    m_mapper          =     vtkSmartPointer<vtkPolyDataMapper>::New();
    m_recordMapper    =     vtkSmartPointer<vtkPolyDataMapper>::New();
    m_actor           =     vtkSmartPointer<vtkActor>::New();
    m_recordActor     =     vtkSmartPointer<vtkActor>::New();
    m_renderer        =     vtkSmartPointer<vtkRenderer>::New();
    vcamera           =     vtkSmartPointer<vtkCamera>::New();
    DeviceModel       =     vtkSmartPointer<vtkSTLReader>::New();
    Devicemapper      =     vtkSmartPointer<vtkPolyDataMapper>::New();
    TextActor         =     vtkSmartPointer<vtkTextActor>::New();
    //定义五个通道所用夹具
    Deviceactor1      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor2      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor3      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor4      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor5      =     vtkSmartPointer<vtkActor>::New();


    DeviceModel->SetFileName("jiazi.stl");//加载夹具模型
    //DeviceModel->Update();
    Devicemapper->SetInputConnection(DeviceModel->GetOutputPort());
    Deviceactor1->SetMapper(Devicemapper);
    Deviceactor2->SetMapper(Devicemapper);
    Deviceactor3->SetMapper(Devicemapper);
    Deviceactor4->SetMapper(Devicemapper);
    Deviceactor5->SetMapper(Devicemapper);
    // 设置透明度为0，即隐藏状态
    Deviceactor1->GetProperty()->SetOpacity(0);
    Deviceactor2->GetProperty()->SetOpacity(0);
    Deviceactor3->GetProperty()->SetOpacity(0);
    Deviceactor4->GetProperty()->SetOpacity(0);
    Deviceactor5->GetProperty()->SetOpacity(0);

    vcamera->SetFocalPoint(0.0, 0.0, 0.0);
    vcamera->SetPosition(3000, 0, -500);
    vcamera->SetClippingRange(50, 4000);
    vcamera->Roll(90);
    m_renderer->SetActiveCamera(vcamera);
    m_renderer->AddActor(m_actor);
    m_renderer->AddActor(m_recordActor);
    m_renderer->AddActor(Deviceactor1);
    m_renderer->AddActor(Deviceactor2);
    m_renderer->AddActor(Deviceactor3);
    m_renderer->AddActor(Deviceactor4);
    m_renderer->AddActor(Deviceactor5);

    // 渲染vtk窗口
    ui->vtkRW->GetRenderWindow()->AddRenderer(m_renderer);

    this->SetLargeGrid();
    this->SetSmallGrid();
    this->SetTranmitter();
    this->SetOrientationArrow();

    // 线程
    TrackWorker = new TrackWork;
    TrackWorkerThread = new QThread(this);//创建子线程
    TrackWorker->moveToThread(TrackWorkerThread);
    qRegisterMetaType<std::vector<TrackingData>>("std::vector<TrackSystem::TrackingData>");

    //  串口
    PortNameList = GetPortNameList();
    ui->selectport->addItems(PortNameList);

    // 信号连接
    connect(ui->zoomin, &QPushButton::clicked, this, &MainWindow::ZoomIn);
    connect(ui->zoomout, &QPushButton::clicked, this, &MainWindow::ZoomOut);
    connect(ui->viewreset, &QPushButton::clicked, this, &MainWindow::ViewReset);
    connect(ui->savebutton, &QPushButton::clicked, this, &MainWindow::SaveFile);
    connect(ui->openport, &QPushButton::clicked, this, &MainWindow::OpenPort);
    connect(ui->closeport, &QPushButton::clicked, this, &MainWindow::ClosePort);
    connect(ui->TrackButton, &SwitchButton::statusChanged, this, &MainWindow::Track);

    connect(this, &MainWindow::StartThread, TrackWorker, &TrackWork::ThreadWorkFunction);//调用线程处理函数
    connect(TrackWorker, &TrackWork::UpdateDataSignal, this, &MainWindow::UpdateData);
    connect(this, &MainWindow::destroyed, this, &MainWindow::CloseUI);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**************************************以下为模型初始化函数*******************************************/

// 渲染粗网格
void MainWindow::SetLargeGrid()
{
    int x[50], y[50], z[50];
    for (int i = 0; i < 50; i++) {
        x[i] = -10000 + i * 400;
        y[i] = 0;
        z[i] = -2000 + i * 400;
    }
    vtkSmartPointer<vtkDoubleArray> xCoords = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> yCoords = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> zCoords = vtkSmartPointer<vtkDoubleArray>::New();
    for (int i = 0; i < 50; i++)
    {
        xCoords->InsertNextValue(x[i]);
        yCoords->InsertNextValue(y[i]);
        zCoords->InsertNextValue(z[i]);
    }

    vtkSmartPointer<vtkRectilinearGrid> rgrid = vtkSmartPointer<vtkRectilinearGrid>::New();
    rgrid->SetDimensions(50, 50, 50);
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    // 从网格中提取一个平面
    vtkSmartPointer<vtkRectilinearGridGeometryFilter> plane = vtkSmartPointer<vtkRectilinearGridGeometryFilter>::New();
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 49, 0, 0, 0, 49);

    vtkSmartPointer<vtkPolyDataMapper> rgridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    rgridMapper->SetInputConnection(plane->GetOutputPort());

    vtkSmartPointer<vtkActor> GridActor = vtkSmartPointer<vtkActor>::New();
    GridActor->SetMapper(rgridMapper);
    GridActor->RotateX(90);
    GridActor->SetPosition(3000, 8000, 0);
    GridActor->GetProperty()->SetColor(107.0 / 255.0, 113.0 / 255.0, 111.0 / 255.0);

    //透明度，0完全透明
    GridActor->GetProperty()->SetOpacity(0.1);
    GridActor->GetProperty()->EdgeVisibilityOn();
    GridActor->GetProperty()->SetEdgeColor(1, 1, 1);
    GridActor->GetProperty()->SetLineWidth(1.0);
    m_renderer->AddActor(GridActor);
}





// 渲染细网格
void MainWindow::SetSmallGrid()
{
    int x[250], y[250], z[250];
    for (int i = 0; i < 250; i++) {
        x[i] = -10000 + i * 80;
        y[i] = 0;
        z[i] = -2000 + i * 80;
    }
    vtkSmartPointer<vtkDoubleArray> xCoords = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> yCoords = vtkSmartPointer<vtkDoubleArray>::New();
    vtkSmartPointer<vtkDoubleArray> zCoords = vtkSmartPointer<vtkDoubleArray>::New();
    for (int i = 0; i < 250; i++)
    {
        xCoords->InsertNextValue(x[i]);
        yCoords->InsertNextValue(y[i]);
        zCoords->InsertNextValue(z[i]);
    }

    vtkSmartPointer<vtkRectilinearGrid> rgrid = vtkSmartPointer<vtkRectilinearGrid>::New();
    rgrid->SetDimensions(250, 250, 250);
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    // 从网格中提取一个平面
    vtkSmartPointer<vtkRectilinearGridGeometryFilter> plane = vtkSmartPointer<vtkRectilinearGridGeometryFilter>::New();
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 249, 0, 0, 0, 249);

    vtkSmartPointer<vtkPolyDataMapper> rgridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    rgridMapper->SetInputConnection(plane->GetOutputPort());

    vtkSmartPointer<vtkActor> GridActor = vtkSmartPointer<vtkActor>::New();
    GridActor->SetMapper(rgridMapper);
    GridActor->RotateX(90);
    GridActor->SetPosition(3000, 8000, 0);
    GridActor->GetProperty()->SetColor(107.0 / 255.0, 113.0 / 255.0, 111.0 / 255.0);

    //透明度，0完全透明
    GridActor->GetProperty()->SetOpacity(0.05);
    GridActor->GetProperty()->EdgeVisibilityOn();
    GridActor->GetProperty()->SetEdgeColor(1, 1, 1);
    GridActor->GetProperty()->SetLineWidth(0.4);
    m_renderer->AddActor(GridActor);
}





// 渲染磁场发射器
void MainWindow::SetTranmitter()
{
    //渲染发射器上盖
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName("shanggai.stl");//读取上盖模型
    reader->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkActor> TransmitterLid = vtkSmartPointer<vtkActor>::New();
    TransmitterLid->SetPosition(0, 0, 2);
    TransmitterLid->RotateX(-90);
    TransmitterLid->RotateY(90);
    TransmitterLid->RotateZ(0);
    TransmitterLid->SetMapper(mapper);
    TransmitterLid->GetProperty()->SetColor(1.0, 1.0, 1.0);
    m_renderer->AddActor(TransmitterLid);

    // 渲染发射器底座
    vtkSmartPointer<vtkSTLReader> reader2 = vtkSmartPointer<vtkSTLReader>::New();
    reader2->SetFileName("dizuo.stl");//读取上盖模型
    reader2->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(reader2->GetOutputPort());

    vtkSmartPointer<vtkActor> TransmitterBottom = vtkSmartPointer<vtkActor>::New();
    TransmitterBottom->SetPosition(0, 0, 0);
    TransmitterBottom->RotateX(-90);
    TransmitterBottom->RotateY(90);
    TransmitterBottom->RotateZ(0);
    TransmitterBottom->SetMapper(mapper2);
    TransmitterBottom->GetProperty()->SetColor(1.0, 1.0, 1.0);
    m_renderer->AddActor(TransmitterBottom);
}





// 渲染方向指示箭头
void MainWindow::SetOrientationArrow()
{
    OrientationArrowActor = vtkSmartPointer<vtkAxesActor>::New();
    OrientationArrowWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

    //加入箭头指示方向
    OrientationArrowWidget->SetOrientationMarker(OrientationArrowActor);
    OrientationArrowWidget->SetInteractor(ui->vtkRW->GetInteractor());
    //OrientationArrowWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
    OrientationArrowWidget->On();
    OrientationArrowWidget->InteractiveOn();
}





// 设置追踪状态文字,初始不显示
void MainWindow::SetText(std::string str)
{
    TextActor->SetInput(str.c_str());
    vtkTextProperty* textprop = TextActor->GetTextProperty();
    textprop->SetFontFamilyToArial();
    textprop->BoldOn();
    textprop->SetFontSize(36);
    textprop->ShadowOn();
    textprop->SetShadowOffset(0, 0);
    textprop->SetColor(40.0 / 255, 200.0 / 255, 62.0 / 255);
    TextActor->SetDisplayPosition(ui->vtkRW->geometry().width() - 200, ui->vtkRW->geometry().height() - 50);
    m_renderer->AddActor(TextActor);
    ui->vtkRW->GetRenderWindow()->Render();
}





//  获取可用串口
QStringList MainWindow::GetPortNameList()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        //qDebug() << "serialPortName:" << info.portName();
    }
    return m_serialPortName;
}
/***********************************************模型初始化函数结束***************************************************/





/**************************************************以下为槽函数*****************************************************/
// 追踪函数
void MainWindow::Track(bool status)
{
    if (status)
    {
        cout << "start track" << endl;
        TrackWorkerThread->start();//启动线程，但是没有启动线程处理函数
        TrackWorker->SetStopFlag(FALSE);
        TrackWorker->OpenPort(ui->selectport->currentText());
        TrackWorker->WritePort();
        this->SetText("TRACKING");
        
        int ch = ui->SelectCH->currentText().toInt();
        switch (ch)
        {
        case 1:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCH1);
            break;
        case 2:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCH2);
            break;
        case 3:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCH3);
            break;
        case 4:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCH4);
            break;
        case 5:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCH5);
            break;
        default:
            TrackWorker->SetWorkMode(TrackWork::WorkMode::EMTTrackCHALL);
            break;
        }
 
        //监听串口，若1s内没有接收到消息则弹窗提示检查是否打开电源
        emit StartThread();
    }
    else
    {
        //关闭追踪
        cout << "stop track" << endl;
        TrackWorker->SetStopFlag(TRUE);
        this->SetText("PAUSE");
        TrackWorker->WritePort();
        TrackWorker->ClosePort();
        TrackWorkerThread->quit();
        TrackWorkerThread->wait();
    }
}


// 刷新显示
void MainWindow::UpdateData(std::vector<TrackingData> datas)
{
    if (!ui->SelectCH->currentText().toInt())//跟踪全部通道
    {
        Deviceactor1->RotateX(-pos5); Deviceactor1->RotateY(-pos4); Deviceactor1->RotateZ(-pos3);
        pos3 = 0; pos4 = 0; pos5 = 0;
        for (int i = 0; i < datas.size(); i++)
        {
            TrackingData datatemp = datas[i];
            // 更新XYZ信息
            ui->positionX->setText(QString::number(datatemp.POSITIONALL[0][0], 'f', 2));
            ui->positionY->setText(QString::number(datatemp.POSITIONALL[0][1], 'f', 2));
            ui->positionZ->setText(QString::number(datatemp.POSITIONALL[0][2], 'f', 2));

            // 更新Angle信息
            ui->anglephi->setText(QString::number(datatemp.POSITIONALL[0][3], 'f', 2));
            ui->angletheta->setText(QString::number(datatemp.POSITIONALL[0][4], 'f', 2));
            ui->anglegamma->setText(QString::number(datatemp.POSITIONALL[0][5], 'f', 2));

            //旋转
            Deviceactor1->RotateX(-pos05);
            Deviceactor1->RotateY(-pos04);
            Deviceactor1->RotateZ(-pos03);
            Deviceactor1->RotateZ(datatemp.POSITIONALL[0][3]);
            Deviceactor1->RotateY(datatemp.POSITIONALL[0][4]);
            Deviceactor1->RotateX(datatemp.POSITIONALL[0][5]);

            Deviceactor2->RotateX(-pos15);
            Deviceactor2->RotateY(-pos14);
            Deviceactor2->RotateZ(-pos13);
            Deviceactor2->RotateZ(datatemp.POSITIONALL[1][3]);
            Deviceactor2->RotateY(datatemp.POSITIONALL[1][4]);
            Deviceactor2->RotateX(datatemp.POSITIONALL[1][5]);

            Deviceactor3->RotateX(-pos25);
            Deviceactor3->RotateY(-pos24);
            Deviceactor3->RotateZ(-pos23);
            Deviceactor3->RotateZ(datatemp.POSITIONALL[2][3]);
            Deviceactor3->RotateY(datatemp.POSITIONALL[2][4]);
            Deviceactor3->RotateX(datatemp.POSITIONALL[2][5]);

            Deviceactor4->RotateX(-pos35);
            Deviceactor4->RotateY(-pos34);
            Deviceactor4->RotateZ(-pos33);
            Deviceactor4->RotateZ(datatemp.POSITIONALL[3][3]);
            Deviceactor4->RotateY(datatemp.POSITIONALL[3][4]);
            Deviceactor4->RotateX(datatemp.POSITIONALL[3][5]);

            Deviceactor5->RotateX(-pos45);
            Deviceactor5->RotateY(-pos44);
            Deviceactor5->RotateZ(-pos43);
            Deviceactor5->RotateZ(datatemp.POSITIONALL[4][3]);
            Deviceactor5->RotateY(datatemp.POSITIONALL[4][4]);
            Deviceactor5->RotateX(datatemp.POSITIONALL[4][5]);

            pos03 = datatemp.POSITIONALL[0][3]; pos04 = datatemp.POSITIONALL[0][4]; pos05 = datatemp.POSITIONALL[0][5];
            pos13 = datatemp.POSITIONALL[1][3]; pos14 = datatemp.POSITIONALL[1][4]; pos15 = datatemp.POSITIONALL[1][5];
            pos23 = datatemp.POSITIONALL[2][3]; pos24 = datatemp.POSITIONALL[2][4]; pos25 = datatemp.POSITIONALL[2][5];
            pos33 = datatemp.POSITIONALL[3][3]; pos34 = datatemp.POSITIONALL[3][4]; pos35 = datatemp.POSITIONALL[3][5];
            pos43 = datatemp.POSITIONALL[4][3]; pos44 = datatemp.POSITIONALL[4][4]; pos45 = datatemp.POSITIONALL[4][5];

            //透明度
            Deviceactor1->GetProperty()->SetOpacity(1);
            Deviceactor2->GetProperty()->SetOpacity(1);
            Deviceactor3->GetProperty()->SetOpacity(1);
            Deviceactor4->GetProperty()->SetOpacity(1);
            Deviceactor5->GetProperty()->SetOpacity(1);

            //位置
            Deviceactor1->SetPosition(datatemp.POSITIONALL[0][0], datatemp.POSITIONALL[0][1], datatemp.POSITIONALL[0][2]);
            Deviceactor2->SetPosition(datatemp.POSITIONALL[1][0], datatemp.POSITIONALL[1][1], datatemp.POSITIONALL[1][2]);
            Deviceactor3->SetPosition(datatemp.POSITIONALL[2][0], datatemp.POSITIONALL[2][1], datatemp.POSITIONALL[2][2]);
            Deviceactor4->SetPosition(datatemp.POSITIONALL[3][0], datatemp.POSITIONALL[3][1], datatemp.POSITIONALL[3][2]);
            Deviceactor5->SetPosition(datatemp.POSITIONALL[4][0], datatemp.POSITIONALL[4][1], datatemp.POSITIONALL[4][2]);
        }
    }
    else if (ui->SelectCH->currentText().toInt())//跟踪单通道
    {
        for (int i = 0; i < datas.size(); i++)
        {
            TrackingData datatemp = datas[i];
            // 更新XYZ信息
            ui->positionX->setText(QString::number(datatemp.POSITION[0], 'f', 2));
            ui->positionY->setText(QString::number(datatemp.POSITION[1], 'f', 2));
            ui->positionZ->setText(QString::number(datatemp.POSITION[2], 'f', 2));

            // 更新Angle信息
            ui->anglephi->setText(QString::number(datatemp.POSITION[3], 'f', 2));
            ui->angletheta->setText(QString::number(datatemp.POSITION[4], 'f', 2));
            ui->anglegamma->setText(QString::number(datatemp.POSITION[5], 'f', 2));

            //显示
            Deviceactor1->RotateX(-pos5);
            Deviceactor1->RotateY(-pos4);
            Deviceactor1->RotateZ(-pos3);
            Deviceactor1->RotateZ(datatemp.POSITION[3]);
            Deviceactor1->RotateY(datatemp.POSITION[4]);
            Deviceactor1->RotateX(datatemp.POSITION[5]);
            pos3 = datatemp.POSITION[3]; pos4 = datatemp.POSITION[4]; pos5 = datatemp.POSITION[5];
            Deviceactor1->GetProperty()->SetOpacity(1);
            Deviceactor1->SetPosition(datatemp.POSITION[0], datatemp.POSITION[1], datatemp.POSITION[2]);
        }
    }
    ui->vtkRW->GetRenderWindow()->Render();
}

// 渲染图视角复位
void MainWindow::ViewReset()
{
    vcamera->SetPosition(3000, 0, -500);
    vcamera->SetRoll(90);
    ui->vtkRW->GetRenderWindow()->Render();
}

// 视图放大
void MainWindow::ZoomIn()
{
    //vcamera->Zoom(1.5);//改变相机焦距
    vcamera->Dolly(1.5);//移动相机位置
    ui->vtkRW->GetRenderWindow()->Render();
}

// 视图缩小
void MainWindow::ZoomOut()
{
    //vcamera->Zoom(0.5);
    vcamera->Dolly(0.6);
    ui->vtkRW->GetRenderWindow()->Render();
}

//  将跟踪数据保存为文件
void MainWindow::SaveFile()//默认保存1000帧的数据
{
    QString path = QFileDialog::getSaveFileName(this, "Save", "../", "CSV(*.csv);;TXT(*.txt)");

    QMessageBox msgbox;

    if (path.isEmpty() == FALSE)//路径不为空
    {
        QFile file;
        file.setFileName(path);//关联文件名字

        bool isOK = file.open(QIODevice::WriteOnly);
        if (isOK == TRUE)
        {
            //执行写文件操作
            for (int i = 0; i < 1000; i++)
            {

            }
        }
        file.close();
        msgbox.information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("保存成功！"));
    }
    else
    {
        msgbox.warning(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("保存失败"));
    }
}

//  打开串口
void MainWindow::OpenPort()
{
    hCom = CreateFile((LPCSTR)ui->selectport->currentText().toLocal8Bit(), //读取COM口
        GENERIC_READ | GENERIC_WRITE,  //允许读和写
        0,  //独占方式
        NULL,
        OPEN_EXISTING,  //打开而不是创建
        0,  //同步方式
        NULL);

    SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024
    COMMTIMEOUTS TimeOuts; //设定读超时
    TimeOuts.ReadIntervalTimeout = 0;
    TimeOuts.ReadTotalTimeoutMultiplier = 0;
    TimeOuts.ReadTotalTimeoutConstant = 0; //设定写超时
    TimeOuts.WriteTotalTimeoutMultiplier = 0;
    TimeOuts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(hCom, &TimeOuts); //设置超时

    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = ui->setboud->currentText().toInt(); //设置波特率
    dcb.ByteSize = 8; //每个字节有8位
    dcb.Parity = NOPARITY; //无奇偶校验位
    dcb.StopBits = ONESTOPBIT; //1个停止位
    SetCommState(hCom, &dcb);
    PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清除输入和输出缓冲区

    if (!SetCommState(hCom, &dcb))
    {
        ui->textEdit->append(QString::fromLocal8Bit("打开串口失败!请检查串口是否连接或被占用"));
    }
    else
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("串口打开成功！波特率为") + ui->setboud->currentText());
    }
}

//  写串口
void MainWindow::WritePort(unsigned char lpOutBuffer[11])
{
    DWORD dwBytesWrite = 11;//要写入的字节数
    COMSTAT ComStat;
    DWORD dwErrorFlags;
    BOOL bWriteStat;
    ClearCommError(hCom, &dwErrorFlags, &ComStat);
    bWriteStat = WriteFile(hCom, lpOutBuffer, dwBytesWrite, &dwBytesWrite, NULL);

    if (!bWriteStat)
    {
        ui->textEdit->append(QString::fromLocal8Bit("写串口失败"));
    }
    else
    {
        ui->textEdit->append(QString::fromLocal8Bit("写串口成功"));
    }

    PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//清除输入输出缓冲区
}

//  关闭串口
void MainWindow::ClosePort()
{    
    if (CloseHandle(hCom) == 0)
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("串口关闭失败！"));
    }
    else
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("串口关闭成功！"));
    }
}

// 关闭UI界面
void MainWindow::CloseUI()
{
    TrackWorker->SetStopFlag(FALSE);
    TrackWorkerThread->quit();
    TrackWorkerThread->wait();
    delete TrackWorker;
}
/*****************************************************************************************************************/
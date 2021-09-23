/***********************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
// 
// function : mainwindow.cpp
// 
// �����ʼ�����������еİ�ť����ǩ
// VTK����Ⱦ��ʾ
// ��Ⱦģ�͵ĳ�ʼ��
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
    //this->setWindowFlags(Qt::FramelessWindowHint);//ȥ�����������

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
    //�������ͨ�����üо�
    Deviceactor1      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor2      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor3      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor4      =     vtkSmartPointer<vtkActor>::New();
    Deviceactor5      =     vtkSmartPointer<vtkActor>::New();


    DeviceModel->SetFileName("jiazi.stl");//���ؼо�ģ��
    //DeviceModel->Update();
    Devicemapper->SetInputConnection(DeviceModel->GetOutputPort());
    Deviceactor1->SetMapper(Devicemapper);
    Deviceactor2->SetMapper(Devicemapper);
    Deviceactor3->SetMapper(Devicemapper);
    Deviceactor4->SetMapper(Devicemapper);
    Deviceactor5->SetMapper(Devicemapper);
    // ����͸����Ϊ0��������״̬
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

    // ��Ⱦvtk����
    ui->vtkRW->GetRenderWindow()->AddRenderer(m_renderer);

    this->SetLargeGrid();
    this->SetSmallGrid();
    this->SetTranmitter();
    this->SetOrientationArrow();

    // �߳�
    TrackWorker = new TrackWork;
    TrackWorkerThread = new QThread(this);//�������߳�
    TrackWorker->moveToThread(TrackWorkerThread);
    qRegisterMetaType<std::vector<TrackingData>>("std::vector<TrackSystem::TrackingData>");

    //  ����
    PortNameList = GetPortNameList();
    ui->selectport->addItems(PortNameList);

    // �ź�����
    connect(ui->zoomin, &QPushButton::clicked, this, &MainWindow::ZoomIn);
    connect(ui->zoomout, &QPushButton::clicked, this, &MainWindow::ZoomOut);
    connect(ui->viewreset, &QPushButton::clicked, this, &MainWindow::ViewReset);
    connect(ui->savebutton, &QPushButton::clicked, this, &MainWindow::SaveFile);
    connect(ui->openport, &QPushButton::clicked, this, &MainWindow::OpenPort);
    connect(ui->closeport, &QPushButton::clicked, this, &MainWindow::ClosePort);
    connect(ui->TrackButton, &SwitchButton::statusChanged, this, &MainWindow::Track);

    connect(this, &MainWindow::StartThread, TrackWorker, &TrackWork::ThreadWorkFunction);//�����̴߳�����
    connect(TrackWorker, &TrackWork::UpdateDataSignal, this, &MainWindow::UpdateData);
    connect(this, &MainWindow::destroyed, this, &MainWindow::CloseUI);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**************************************����Ϊģ�ͳ�ʼ������*******************************************/

// ��Ⱦ������
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

    // ����������ȡһ��ƽ��
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

    //͸���ȣ�0��ȫ͸��
    GridActor->GetProperty()->SetOpacity(0.1);
    GridActor->GetProperty()->EdgeVisibilityOn();
    GridActor->GetProperty()->SetEdgeColor(1, 1, 1);
    GridActor->GetProperty()->SetLineWidth(1.0);
    m_renderer->AddActor(GridActor);
}





// ��Ⱦϸ����
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

    // ����������ȡһ��ƽ��
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

    //͸���ȣ�0��ȫ͸��
    GridActor->GetProperty()->SetOpacity(0.05);
    GridActor->GetProperty()->EdgeVisibilityOn();
    GridActor->GetProperty()->SetEdgeColor(1, 1, 1);
    GridActor->GetProperty()->SetLineWidth(0.4);
    m_renderer->AddActor(GridActor);
}





// ��Ⱦ�ų�������
void MainWindow::SetTranmitter()
{
    //��Ⱦ�������ϸ�
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName("shanggai.stl");//��ȡ�ϸ�ģ��
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

    // ��Ⱦ����������
    vtkSmartPointer<vtkSTLReader> reader2 = vtkSmartPointer<vtkSTLReader>::New();
    reader2->SetFileName("dizuo.stl");//��ȡ�ϸ�ģ��
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





// ��Ⱦ����ָʾ��ͷ
void MainWindow::SetOrientationArrow()
{
    OrientationArrowActor = vtkSmartPointer<vtkAxesActor>::New();
    OrientationArrowWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

    //�����ͷָʾ����
    OrientationArrowWidget->SetOrientationMarker(OrientationArrowActor);
    OrientationArrowWidget->SetInteractor(ui->vtkRW->GetInteractor());
    //OrientationArrowWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
    OrientationArrowWidget->On();
    OrientationArrowWidget->InteractiveOn();
}





// ����׷��״̬����,��ʼ����ʾ
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





//  ��ȡ���ô���
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
/***********************************************ģ�ͳ�ʼ����������***************************************************/





/**************************************************����Ϊ�ۺ���*****************************************************/
// ׷�ٺ���
void MainWindow::Track(bool status)
{
    if (status)
    {
        cout << "start track" << endl;
        TrackWorkerThread->start();//�����̣߳�����û�������̴߳�����
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
 
        //�������ڣ���1s��û�н��յ���Ϣ�򵯴���ʾ����Ƿ�򿪵�Դ
        emit StartThread();
    }
    else
    {
        //�ر�׷��
        cout << "stop track" << endl;
        TrackWorker->SetStopFlag(TRUE);
        this->SetText("PAUSE");
        TrackWorker->WritePort();
        TrackWorker->ClosePort();
        TrackWorkerThread->quit();
        TrackWorkerThread->wait();
    }
}


// ˢ����ʾ
void MainWindow::UpdateData(std::vector<TrackingData> datas)
{
    if (!ui->SelectCH->currentText().toInt())//����ȫ��ͨ��
    {
        Deviceactor1->RotateX(-pos5); Deviceactor1->RotateY(-pos4); Deviceactor1->RotateZ(-pos3);
        pos3 = 0; pos4 = 0; pos5 = 0;
        for (int i = 0; i < datas.size(); i++)
        {
            TrackingData datatemp = datas[i];
            // ����XYZ��Ϣ
            ui->positionX->setText(QString::number(datatemp.POSITIONALL[0][0], 'f', 2));
            ui->positionY->setText(QString::number(datatemp.POSITIONALL[0][1], 'f', 2));
            ui->positionZ->setText(QString::number(datatemp.POSITIONALL[0][2], 'f', 2));

            // ����Angle��Ϣ
            ui->anglephi->setText(QString::number(datatemp.POSITIONALL[0][3], 'f', 2));
            ui->angletheta->setText(QString::number(datatemp.POSITIONALL[0][4], 'f', 2));
            ui->anglegamma->setText(QString::number(datatemp.POSITIONALL[0][5], 'f', 2));

            //��ת
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

            //͸����
            Deviceactor1->GetProperty()->SetOpacity(1);
            Deviceactor2->GetProperty()->SetOpacity(1);
            Deviceactor3->GetProperty()->SetOpacity(1);
            Deviceactor4->GetProperty()->SetOpacity(1);
            Deviceactor5->GetProperty()->SetOpacity(1);

            //λ��
            Deviceactor1->SetPosition(datatemp.POSITIONALL[0][0], datatemp.POSITIONALL[0][1], datatemp.POSITIONALL[0][2]);
            Deviceactor2->SetPosition(datatemp.POSITIONALL[1][0], datatemp.POSITIONALL[1][1], datatemp.POSITIONALL[1][2]);
            Deviceactor3->SetPosition(datatemp.POSITIONALL[2][0], datatemp.POSITIONALL[2][1], datatemp.POSITIONALL[2][2]);
            Deviceactor4->SetPosition(datatemp.POSITIONALL[3][0], datatemp.POSITIONALL[3][1], datatemp.POSITIONALL[3][2]);
            Deviceactor5->SetPosition(datatemp.POSITIONALL[4][0], datatemp.POSITIONALL[4][1], datatemp.POSITIONALL[4][2]);
        }
    }
    else if (ui->SelectCH->currentText().toInt())//���ٵ�ͨ��
    {
        for (int i = 0; i < datas.size(); i++)
        {
            TrackingData datatemp = datas[i];
            // ����XYZ��Ϣ
            ui->positionX->setText(QString::number(datatemp.POSITION[0], 'f', 2));
            ui->positionY->setText(QString::number(datatemp.POSITION[1], 'f', 2));
            ui->positionZ->setText(QString::number(datatemp.POSITION[2], 'f', 2));

            // ����Angle��Ϣ
            ui->anglephi->setText(QString::number(datatemp.POSITION[3], 'f', 2));
            ui->angletheta->setText(QString::number(datatemp.POSITION[4], 'f', 2));
            ui->anglegamma->setText(QString::number(datatemp.POSITION[5], 'f', 2));

            //��ʾ
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

// ��Ⱦͼ�ӽǸ�λ
void MainWindow::ViewReset()
{
    vcamera->SetPosition(3000, 0, -500);
    vcamera->SetRoll(90);
    ui->vtkRW->GetRenderWindow()->Render();
}

// ��ͼ�Ŵ�
void MainWindow::ZoomIn()
{
    //vcamera->Zoom(1.5);//�ı��������
    vcamera->Dolly(1.5);//�ƶ����λ��
    ui->vtkRW->GetRenderWindow()->Render();
}

// ��ͼ��С
void MainWindow::ZoomOut()
{
    //vcamera->Zoom(0.5);
    vcamera->Dolly(0.6);
    ui->vtkRW->GetRenderWindow()->Render();
}

//  ���������ݱ���Ϊ�ļ�
void MainWindow::SaveFile()//Ĭ�ϱ���1000֡������
{
    QString path = QFileDialog::getSaveFileName(this, "Save", "../", "CSV(*.csv);;TXT(*.txt)");

    QMessageBox msgbox;

    if (path.isEmpty() == FALSE)//·����Ϊ��
    {
        QFile file;
        file.setFileName(path);//�����ļ�����

        bool isOK = file.open(QIODevice::WriteOnly);
        if (isOK == TRUE)
        {
            //ִ��д�ļ�����
            for (int i = 0; i < 1000; i++)
            {

            }
        }
        file.close();
        msgbox.information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����ɹ���"));
    }
    else
    {
        msgbox.warning(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����ʧ��"));
    }
}

//  �򿪴���
void MainWindow::OpenPort()
{
    hCom = CreateFile((LPCSTR)ui->selectport->currentText().toLocal8Bit(), //��ȡCOM��
        GENERIC_READ | GENERIC_WRITE,  //�������д
        0,  //��ռ��ʽ
        NULL,
        OPEN_EXISTING,  //�򿪶����Ǵ���
        0,  //ͬ����ʽ
        NULL);

    SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024
    COMMTIMEOUTS TimeOuts; //�趨����ʱ
    TimeOuts.ReadIntervalTimeout = 0;
    TimeOuts.ReadTotalTimeoutMultiplier = 0;
    TimeOuts.ReadTotalTimeoutConstant = 0; //�趨д��ʱ
    TimeOuts.WriteTotalTimeoutMultiplier = 0;
    TimeOuts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ

    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = ui->setboud->currentText().toInt(); //���ò�����
    dcb.ByteSize = 8; //ÿ���ֽ���8λ
    dcb.Parity = NOPARITY; //����żУ��λ
    dcb.StopBits = ONESTOPBIT; //1��ֹͣλ
    SetCommState(hCom, &dcb);
    PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//�����������������

    if (!SetCommState(hCom, &dcb))
    {
        ui->textEdit->append(QString::fromLocal8Bit("�򿪴���ʧ��!���鴮���Ƿ����ӻ�ռ��"));
    }
    else
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("���ڴ򿪳ɹ���������Ϊ") + ui->setboud->currentText());
    }
}

//  д����
void MainWindow::WritePort(unsigned char lpOutBuffer[11])
{
    DWORD dwBytesWrite = 11;//Ҫд����ֽ���
    COMSTAT ComStat;
    DWORD dwErrorFlags;
    BOOL bWriteStat;
    ClearCommError(hCom, &dwErrorFlags, &ComStat);
    bWriteStat = WriteFile(hCom, lpOutBuffer, dwBytesWrite, &dwBytesWrite, NULL);

    if (!bWriteStat)
    {
        ui->textEdit->append(QString::fromLocal8Bit("д����ʧ��"));
    }
    else
    {
        ui->textEdit->append(QString::fromLocal8Bit("д���ڳɹ�"));
    }

    PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//����������������
}

//  �رմ���
void MainWindow::ClosePort()
{    
    if (CloseHandle(hCom) == 0)
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("���ڹر�ʧ�ܣ�"));
    }
    else
    {
        ui->textEdit->append(ui->selectport->currentText() + QString::fromLocal8Bit("���ڹرճɹ���"));
    }
}

// �ر�UI����
void MainWindow::CloseUI()
{
    TrackWorker->SetStopFlag(FALSE);
    TrackWorkerThread->quit();
    TrackWorkerThread->wait();
    delete TrackWorker;
}
/*****************************************************************************************************************/
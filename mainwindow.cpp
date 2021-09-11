/***********************************************************************************************/
// function : mainwindow.cpp
// 
// �����ʼ�����������еİ�ť����ǩ
// VTK����Ⱦ��ʾ
// ��Ⱦģ�͵ĳ�ʼ��
//
/***********************************************************************************************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <string>

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


    //�ź�����
    connect(ui->zoomin, &QPushButton::clicked, this, &MainWindow::ZoomIn);
    connect(ui->zoomout, &QPushButton::clicked, this, &MainWindow::ZoomOut);
    connect(ui->viewreset, &QPushButton::clicked, this, &MainWindow::ViewReset);
    connect(ui->savebutton, &QPushButton::clicked, this, &MainWindow::SaveFile);
    connect(ui->openport, &QPushButton::clicked, this, &MainWindow::OpenPort);
    connect(ui->closeport, &QPushButton::clicked, this, &MainWindow::ClosePort);
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
    textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput(str.c_str());
    vtkTextProperty* textprop = textActor->GetTextProperty();
    textprop->SetFontFamilyToArial();
    textprop->BoldOn();
    textprop->SetFontSize(36);
    textprop->ShadowOn();
    textprop->SetShadowOffset(0, 0);
    textprop->SetColor(40.0 / 255, 200.0 / 255, 62.0 / 255);
    textActor->SetDisplayPosition(ui->vtkRW->geometry().width() - 160, ui->vtkRW->geometry().height() - 50);
    m_renderer->AddActor(textActor);
}
/***********************************************ģ�ͳ�ʼ����������***************************************************/





/**************************************************����Ϊ�ۺ���*****************************************************/
// ��ʼ׷��
void MainWindow::StartTrack()
{

}

// ����׷������
void MainWindow::OnSave()
{

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
void MainWindow::SaveFile()
{
    QString path = QFileDialog::getSaveFileName(this, "Save", "../", "TXT(*.txt);;CSV(*.csv)");
    if (path.isEmpty() == false)//·����Ϊ��
    {
        QFile file;
        file.setFileName(path);//�����ļ�����

        bool isOK = file.open(QIODevice::WriteOnly);
        if (isOK == true)
        {
            //ִ��д�ļ�����

        }
        file.close();
    }
}

//  �򿪴���
HANDLE hCom;
void MainWindow::OpenPort()
{
    hCom = CreateFile((LPCSTR)ui->chooseport->currentText().toLocal8Bit(), //��ȡCOM��
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
        ui->textEdit->append(ui->chooseport->currentText() + QString::fromLocal8Bit("���ڴ򿪳ɹ���������Ϊ") + ui->setboud->currentText());
    }
}

//  �رմ���
void MainWindow::ClosePort()
{    
    if (CloseHandle(hCom) == 0)
    {
        ui->textEdit->append(ui->chooseport->currentText() + QString::fromLocal8Bit("���ڹر�ʧ�ܣ�"));
    }
    else
    {
        ui->textEdit->append(ui->chooseport->currentText() + QString::fromLocal8Bit("���ڹرճɹ���"));
    }
}
/*****************************************************************************************************************/
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

#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EMT-show");
    //this->setWindowFlags(Qt::FramelessWindowHint);//ȥ�����������

    m_polydata = vtkSmartPointer<vtkPolyData>::New();
    m_recordPolydata = vtkSmartPointer<vtkPolyData>::New();
    m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_recordMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_recordActor = vtkSmartPointer<vtkActor>::New();
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    // ����ָʾ��ͷ
    AxesActor = vtkSmartPointer<vtkAxesActor>::New();
    AxesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    vcamera = vtkSmartPointer<vtkCamera>::New();
    DeviceModel = vtkSmartPointer<vtkSTLReader>::New();
    Devicemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //�������ͨ�����üо�
    Deviceactor1 = vtkSmartPointer<vtkActor>::New();
    Deviceactor2 = vtkSmartPointer<vtkActor>::New();
    Deviceactor3 = vtkSmartPointer<vtkActor>::New();
    Deviceactor4 = vtkSmartPointer<vtkActor>::New();
    Deviceactor5 = vtkSmartPointer<vtkActor>::New();


    DeviceModel->SetFileName("jiazi.stl");//���ؼо�ģ��
    DeviceModel->Update();
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

    // ��Ⱦvtk����
    ui->vtkRW->GetRenderWindow()->AddRenderer(m_renderer);

    //�����ͷָʾ����
    AxesWidget->SetOrientationMarker(AxesActor);
    AxesWidget->SetInteractor(ui->vtkRW->GetInteractor());
    AxesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
    AxesWidget->On();
    AxesWidget->InteractiveOn();

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

    this->SetLargeGrid();
    this->SetSmallGrid();
    this->SetTranmitter();
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
    vtkNew<vtkDoubleArray> xCoords;
    vtkNew<vtkDoubleArray> yCoords;
    vtkNew<vtkDoubleArray> zCoords;
    for (int i = 0; i < 50; i++)
    {
        xCoords->InsertNextValue(x[i]);
        yCoords->InsertNextValue(y[i]);
        zCoords->InsertNextValue(z[i]);
    }

    vtkNew<vtkRectilinearGrid> rgrid;
    rgrid->SetDimensions(50, 50, 50);
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    // ����������ȡһ��ƽ��
    vtkNew<vtkRectilinearGridGeometryFilter> plane;
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 49, 0, 0, 0, 49);

    vtkNew<vtkPolyDataMapper> rgridMapper;
    rgridMapper->SetInputConnection(plane->GetOutputPort());

    vtkNew<vtkActor> GridActor;
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
    vtkNew<vtkDoubleArray> xCoords;
    vtkNew<vtkDoubleArray> yCoords;
    vtkNew<vtkDoubleArray> zCoords;
    for (int i = 0; i < 250; i++)
    {
        xCoords->InsertNextValue(x[i]);
        yCoords->InsertNextValue(y[i]);
        zCoords->InsertNextValue(z[i]);
    }

    vtkNew<vtkRectilinearGrid> rgrid;
    rgrid->SetDimensions(250, 250, 250);
    rgrid->SetXCoordinates(xCoords);
    rgrid->SetYCoordinates(yCoords);
    rgrid->SetZCoordinates(zCoords);

    // ����������ȡһ��ƽ��
    vtkNew<vtkRectilinearGridGeometryFilter> plane;
    plane->SetInputData(rgrid);
    plane->SetExtent(0, 249, 0, 0, 0, 249);

    vtkNew<vtkPolyDataMapper> rgridMapper;
    rgridMapper->SetInputConnection(plane->GetOutputPort());

    vtkNew<vtkActor> GridActor;
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

    vtkNew<vtkPolyDataMapper> mapper0;
    mapper0->SetInputConnection(reader2->GetOutputPort());

    vtkSmartPointer<vtkActor> TransmitterBottom = vtkSmartPointer<vtkActor>::New();
    TransmitterBottom->SetPosition(0, 0, 0);
    TransmitterBottom->RotateX(-90);
    TransmitterBottom->RotateY(90);
    TransmitterBottom->RotateZ(0);
    TransmitterBottom->SetMapper(mapper0);
    TransmitterBottom->GetProperty()->SetColor(1.0, 1.0, 1.0);
    m_renderer->AddActor(TransmitterBottom);
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
/********************************************ģ�ͳ�ʼ����������******************************************************/

// ��ʼ׷��
void MainWindow::StartTrack()
{

}

// ����׷������
void MainWindow::OnSave()
{

}

// �˳�UI����
void MainWindow::exitUI()
{

}

// ��Ⱦͼ�ӽǸ�λ
void MainWindow::vtkViewReset()
{
    vcamera->SetPosition(3000, 0, -500);
    vcamera->SetRoll(90);
    ui->vtkRW->GetRenderWindow()->Render();
}
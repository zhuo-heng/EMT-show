/***********************************************************************************************/
// function : mainwindow.cpp
// 
// 负责初始化主窗口所有的按钮、标签
// VTK的渲染显示
// 渲染模型的初始化
//
/***********************************************************************************************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>

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


    //信号连接
    connect(ui->zoomin, &QPushButton::clicked, this, &MainWindow::ZoomIn);
    connect(ui->zoomout, &QPushButton::clicked, this, &MainWindow::ZoomOut);
    connect(ui->viewreset, &QPushButton::clicked, this, &MainWindow::ViewReset);
    connect(ui->savebutton, &QPushButton::clicked, this, &MainWindow::SaveFile);
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
/***********************************************模型初始化函数结束***************************************************/





/**************************************************以下为槽函数*****************************************************/
// 开始追踪
void MainWindow::StartTrack()
{

}

// 保存追踪数据
void MainWindow::OnSave()
{

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
void MainWindow::SaveFile()
{
    QString path = QFileDialog::getSaveFileName(this, "Save", "../", "TXT(*.txt);;CSV(*.csv)");
    if (path.isEmpty() == false)//路径不为空
    {
        QFile file;
        file.setFileName(path);//关联文件名字

        bool isOK = file.open(QIODevice::WriteOnly);
        if (isOK == true)
        {
            //执行写文件操作

        }
        file.close();
    }
}
/*****************************************************************************************************************/
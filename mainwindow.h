#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVTKWidget.h>
#include <iostream>

//VTK
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDoubleArray.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkOBJReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSphereSource.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkSTLReader.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void StartTrack();
    void OnSave();
    void ViewReset();
    void ZoomIn();
    void ZoomOut();
    void SaveFile();

private:
    Ui::MainWindow *ui;

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkActor> m_actor;
    vtkSmartPointer<vtkActor> m_recordActor;
    vtkSmartPointer<vtkPolyData> m_polydata;
    vtkSmartPointer<vtkPolyData> m_recordPolydata;
    vtkSmartPointer<vtkPolyDataMapper> m_mapper;
    vtkSmartPointer<vtkPolyDataMapper> m_recordMapper;

    vtkSmartPointer<vtkCamera> vcamera;
    vtkSmartPointer<vtkTextActor> textActor;
    vtkSmartPointer<vtkSTLReader> DeviceModel;
    vtkSmartPointer<vtkPolyDataMapper> Devicemapper;
    vtkSmartPointer<vtkActor> Deviceactor1;
    vtkSmartPointer<vtkActor> Deviceactor2;
    vtkSmartPointer<vtkActor> Deviceactor3;
    vtkSmartPointer<vtkActor> Deviceactor4;
    vtkSmartPointer<vtkActor> Deviceactor5;
    vtkSmartPointer<vtkAxesActor> OrientationArrowActor;
    vtkSmartPointer<vtkOrientationMarkerWidget> OrientationArrowWidget;

    void SetSmallGrid();
    void SetLargeGrid();
    void SetTranmitter();
    void SetOrientationArrow();
    void SetText(std::string);
};
#endif // MAINWINDOW_H

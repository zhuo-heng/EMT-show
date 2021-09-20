#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVTKWidget.h>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <iostream>
#include <Windows.h>
#include <string>
#include "TrackWork.h"

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

    double pos3 = 0, pos4 = 0, pos5 = 0;
    double pos03 = 0, pos04 = 0, pos05 = 0;
    double pos13 = 0, pos14 = 0, pos15 = 0;
    double pos23 = 0, pos24 = 0, pos25 = 0;
    double pos33 = 0, pos34 = 0, pos35 = 0;
    double pos43 = 0, pos44 = 0, pos45 = 0;

protected slots:
    void Track(bool status);
    void ViewReset();
    void ZoomIn();
    void ZoomOut();
    void SaveFile();
    void OpenPort();
    void ClosePort();

    void UpdateData(std::vector<TrackingData> datas);
    void CloseUI();

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
    vtkSmartPointer<vtkTextActor> TextActor;
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

    HANDLE hCom;
    QThread* TrackWorkerThread;
    TrackWork* TrackWorker;

signals:
    void StartThread();
};
#endif // MAINWINDOW_H

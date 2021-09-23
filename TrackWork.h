#ifndef TRACKWORK_H
#define TRACKWORK_H

#include <QObject>
#include <QThread>
#include <QMetaType>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QString>
#include <Windows.h>

//Q_DECLARE_METATYPE(std::vector<TrackingData>);

struct TrackingData
{
	double POSITION[6];
	double POSITIONALL[5][6];
};

class TrackWork :
    public QObject
{
    Q_OBJECT

public:
	explicit TrackWork(QObject* parent = 0);
	~TrackWork();

	// 跟踪通道
	enum class WorkMode//后续添加新功能时，在这里添加并调用即可
	{
		EMTTrackCH1 = 1,
		EMTTrackCH2 = 2,
		EMTTrackCH3 = 3,
		EMTTrackCH4 = 4,
		EMTTrackCH5 = 5,
		EMTTrackCHALL = 0
	};

	void SetStopFlag(BOOL flag);
	void SetWorkMode(WorkMode w);//设置工作模式
	void ThreadWorkFunction();//线程工作函数
	void OpenPort(QString COM);
	void WritePort();
	void ClosePort();
	void ReadPort(int CH);
signals:
	void UpdateDataSignal(std::vector<TrackingData> datas);

private:
	WorkMode m_WorkMode;
	QSerialPort* SerialPort;
	bool IsStop;
	unsigned char StartCommand[11] = { 0xA5,0x5A,0x01,0x01,0x00,0x01,0x00,0xE1,0xF0,0xB6,0x6B };
	unsigned char StopCommand[11] = { 0xA5,0x5A,0x01,0x03,0x00,0x01,0x00,0xE1,0xF0,0xB6,0x6B };
	QByteArray QStartCommand = QByteArray::fromRawData((char*)StartCommand, sizeof(StartCommand));
	QByteArray QStopCommand = QByteArray::fromRawData((char*)StopCommand, sizeof(StopCommand));
	QByteArray framehead;
	QByteArray framehead1;
};

#endif
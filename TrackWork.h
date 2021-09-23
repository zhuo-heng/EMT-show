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

	// ����ͨ��
	enum class WorkMode//��������¹���ʱ����������Ӳ����ü���
	{
		EMTTrackCH1 = 1,
		EMTTrackCH2 = 2,
		EMTTrackCH3 = 3,
		EMTTrackCH4 = 4,
		EMTTrackCH5 = 5,
		EMTTrackCHALL = 0
	};

	void SetStopFlag(BOOL flag);
	void SetWorkMode(WorkMode w);//���ù���ģʽ
	void ThreadWorkFunction();//�̹߳�������
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
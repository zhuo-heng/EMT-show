#ifndef TRACKWORK_H
#define TRACKWORK_H

#include <QObject>
#include <QThread>
#include <Windows.h>

class TrackWork :
    public QObject
{
    Q_OBJECT

public:
	explicit TrackWork(QObject* parent = 0);
	~TrackWork();
	// ����ͨ��
	enum WorkMode//��������¹���ʱ����������Ӳ����ü���
	{
		EMTTrackCH1 = 1,
		EMTTrackCH2 = 2,
		EMTTrackCH3 = 3,
		EMTTrackCH4 = 4,
		EMTTrackCH5 = 5,
		EMTTrackCHALL = 0,
	};

	void GetData(int CH);
	void SetStopFlag(BOOL flag);
	void SetWorkMode(WorkMode w);//���ù���ģʽ
	void ThreadWorkFunction();//�̹߳�������

protected:
	WorkMode m_WorkMode;

	void OpenPort();//�򿪴���
	void ConfigPort();//���ô���
	void WPort();//д��������
	void RPort(int CH);// ����������
	void ClosePort();//�رմ���
	bool IsStop;
};

#endif
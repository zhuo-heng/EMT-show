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
	// 跟踪通道
	enum WorkMode//后续添加新功能时，在这里添加并调用即可
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
	void SetWorkMode(WorkMode w);//设置工作模式
	void ThreadWorkFunction();//线程工作函数

protected:
	WorkMode m_WorkMode;

	void OpenPort();//打开串口
	void ConfigPort();//配置串口
	void WPort();//写串口数据
	void RPort(int CH);// 读串口数据
	void ClosePort();//关闭串口
	bool IsStop;
};

#endif
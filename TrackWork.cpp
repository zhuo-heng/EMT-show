/***********************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
// 
// function : TrackWork.cpp
// 
// ������
// �������ͨ��������λ������
/***********************************************************************************************/
#include "TrackWork.h"

#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

TrackWork::TrackWork(QObject* parent) : QObject(parent)
{
    IsStop = FALSE;
    SerialPort = new QSerialPort();
    m_WorkMode = WorkMode::EMTTrackCH1;
}

TrackWork::~TrackWork()
{
    if (SerialPort->isOpen())
    {
        this->WritePort();
        SerialPort->close();
    }
    delete SerialPort;
}

//  �̴߳�����(���ݲ�ͬ�Ĺ���ģʽ���ö�Ӧ�Ĵ�����)
void TrackWork::ThreadWorkFunction()
{
    //this->OpenPort();
    //this->WritePort();
    switch (m_WorkMode)
    {
    case WorkMode::EMTTrackCH1:
        this->ReadPort(1);
        break;
    case WorkMode::EMTTrackCH2:
        this->ReadPort(2);
        break;
    case WorkMode::EMTTrackCH3:
        this->ReadPort(3);
        break;
    case WorkMode::EMTTrackCH4:
        this->ReadPort(4);
        break;
    case WorkMode::EMTTrackCH5:
        this->ReadPort(5);
        break;
    case WorkMode::EMTTrackCHALL:
        this->ReadPort(0);
        break;
    default:
        break;
    }
}

//  ���ù���ģʽ
void TrackWork::SetWorkMode(WorkMode w)
{
    m_WorkMode = w;
}

//  �����̹߳رձ�־λ
void TrackWork::SetStopFlag(BOOL flag)
{
    IsStop = flag;
}

//  �򿪴���
void TrackWork::OpenPort(QString COM)
{
    if (SerialPort->isOpen())//��������Ѿ����� �ȸ����ر���
    {
        SerialPort->clear();
        SerialPort->close();
    }

    SerialPort->setPortName(COM);
    SerialPort->setBaudRate(230400, QSerialPort::AllDirections);//���ò����ʺͶ�д����
    SerialPort->setDataBits(QSerialPort::Data8);      //����λΪ8λ
    SerialPort->setFlowControl(QSerialPort::NoFlowControl);//��������
    SerialPort->setParity(QSerialPort::NoParity); //��У��λ
    SerialPort->setStopBits(QSerialPort::OneStop); //1λֹͣλ

    if (!SerialPort->open(QIODevice::ReadWrite))//��ReadWrite ��ģʽ���Դ򿪴���
    {
        cout << "no!";
        return;
    }
    else
        cout << "yes!";
}

//  �رմ���
void TrackWork::ClosePort()
{
    SerialPort->close();
}

//  д�봮��
void TrackWork::WritePort()
{
    if (IsStop == TRUE)
        SerialPort->write(QStopCommand);
    else
        SerialPort->write(QStartCommand);
}

void TrackWork::ReadPort(int CH)
{
    //unsigned char *str;
    //unsigned char* framehead;
    //unsigned char* framehead1;

    //QString str(SerialPort->readAll());
    while (IsStop == FALSE)
    {
        //str = SerialPort->read(2);
        framehead = SerialPort->readAll();
        if (framehead.toHex() != "a55a020200c0") //У��֡ͷ,ʶ����,������,���ݸ���
        {
            continue;
        }
        qDebug() << framehead.toHex();
        //framehead1 = SerialPort->readAll();
        //if (framehead1.toHex() != "020200c0") //ʶ����,������,���ݸ���
        //{
        //    continue;
        //}
        //qDebug() << framehead1.toHex();
        //str = reinterpret_cast<unsigned char*>(SerialPort->read(196).data());
        //if (str[194] != 0xB6 || str[195] != 0x6B)//У��֡β
        //{
        //    continue;
        //}

        /*
        if (CH != 0)
        {
            double pos[6];//�ֱ���[X,Y,Z,PHI,THETA,GAMMA]
            switch (CH)
            {
            case 1:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 38]) + (str[i * 4 + 1 + 38] << 8) + (str[i * 4 + 2 + 38] << 16)
                        + (str[i * 4 + 3 + 38] << 24)) / 10000.0;
                    cout << pos[i] << " ";//����������
                }
                cout << endl;
                break;
            case 2:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 64]) + (str[i * 4 + 1 + 64] << 8) + (str[i * 4 + 2 + 64] << 16)
                        + (str[i * 4 + 3 + 64] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 3:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 90]) + (str[i * 4 + 1 + 90] << 8) + (str[i * 4 + 2 + 90] << 16)
                        + (str[i * 4 + 3 + 90] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 4:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 116]) + (str[i * 4 + 1 + 116] << 8) + (str[i * 4 + 2 + 116] << 16)
                        + (str[i * 4 + 3 + 116] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 5:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 142]) + (str[i * 4 + 1 + 142] << 8) + (str[i * 4 + 2 + 142] << 16)
                        + (str[i * 4 + 3 + 142] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            default:
                break;
            }

            std::vector<TrackingData> trackingdatas;

            TrackingData DataTemp;
            DataTemp.POSITION[0] = pos[0];
            DataTemp.POSITION[1] = pos[1];
            DataTemp.POSITION[2] = pos[2];
            DataTemp.POSITION[3] = pos[3];
            DataTemp.POSITION[4] = pos[4];
            DataTemp.POSITION[5] = pos[5];
            trackingdatas.push_back(DataTemp);

            //vtkPolyData* polydatatemp = vtkPolyData::New();
            emit UpdateDataSignal(trackingdatas);
            //break;//ɾ���������һֱȡ��
        }
        else
        {
            double pos[5][6];//�ֱ���[X,Y,Z,PHI,THETA,GAMMA]
            for (int i = 0; i < 6; i++)
            {
                pos[0][i] = ((str[i * 4 + 38]) + (str[i * 4 + 1 + 38] << 8) + (str[i * 4 + 2 + 38] << 16)
                    + (str[i * 4 + 3 + 38] << 24)) / 10000.0;
                pos[1][i] = ((str[i * 4 + 64]) + (str[i * 4 + 1 + 64] << 8) + (str[i * 4 + 2 + 64] << 16)
                    + (str[i * 4 + 3 + 64] << 24)) / 10000.0;
                pos[2][i] = ((str[i * 4 + 90]) + (str[i * 4 + 1 + 90] << 8) + (str[i * 4 + 2 + 90] << 16)
                    + (str[i * 4 + 3 + 90] << 24)) / 10000.0;
                pos[3][i] = ((str[i * 4 + 116]) + (str[i * 4 + 1 + 116] << 8) + (str[i * 4 + 2 + 116] << 16)
                    + (str[i * 4 + 3 + 116] << 24)) / 10000.0;
                pos[4][i] = ((str[i * 4 + 142]) + (str[i * 4 + 1 + 142] << 8) + (str[i * 4 + 2 + 142] << 16)
                    + (str[i * 4 + 3 + 142] << 24)) / 10000.0;
            }

            std::vector<TrackingData> trackingdatas;
            TrackingData DataTemp;
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    DataTemp.POSITIONALL[i][j] = pos[i][j];
                }
            }

            //std::cout << datatemp.POSITIONALL[0][0] << " " << datatemp.POSITIONALL[0][1] << " " << datatemp.POSITIONALL[0][2] << " " << datatemp.POSITIONALL[0][3] << " " << datatemp.POSITIONALL[0][4] << " " << datatemp.POSITIONALL[0][5] << endl;
            //std::cout << datatemp.POSITIONALL[1][0] << " " << datatemp.POSITIONALL[1][1] << " " << datatemp.POSITIONALL[1][2] << " " << datatemp.POSITIONALL[1][3] << " " << datatemp.POSITIONALL[1][4] << " " << datatemp.POSITIONALL[1][5] << endl;
            //std::cout << datatemp.POSITIONALL[2][0] << " " << datatemp.POSITIONALL[2][1] << " " << datatemp.POSITIONALL[2][2] << " " << datatemp.POSITIONALL[2][3] << " " << datatemp.POSITIONALL[2][4] << " " << datatemp.POSITIONALL[2][5] << endl;
            //std::cout << datatemp.POSITIONALL[3][0] << " " << datatemp.POSITIONALL[3][1] << " " << datatemp.POSITIONALL[3][2] << " " << datatemp.POSITIONALL[3][3] << " " << datatemp.POSITIONALL[3][4] << " " << datatemp.POSITIONALL[3][5] << endl;
            //std::cout << datatemp.POSITIONALL[4][0] << " " << datatemp.POSITIONALL[4][1] << " " << datatemp.POSITIONALL[4][2] << " " << datatemp.POSITIONALL[4][3] << " " << datatemp.POSITIONALL[4][4] << " " << datatemp.POSITIONALL[4][5] << endl;
            //std::cout << std::endl;

            trackingdatas.push_back(DataTemp);

            //vtkPolyData* polydatatemp = vtkPolyData::New();
            emit UpdateDataSignal(trackingdatas);
            //break;//ɾ���������һֱȡ��
        }
        if (IsStop == TRUE)
        {
            break;
        }*/
    }
}
/*
//  ��ȡ��������
void TrackWork::RPort(int CH)
{
    unsigned char str[196];
    DWORD readsize = 196;//��ȡ���ֽ���
    DWORD dwError;
    DWORD hdrsize = 1;
    DWORD hdrsize1 = 4;
    BOOL bReadStat;
    COMSTAT cs;
    WaitForSingleObject(hCom, INFINITE);
    ClearCommError(hCom, &dwError, &cs);//ȡ��״̬
    //�ж������Ƿ������׼��������
    if (cs.cbInQue > sizeof(str))
    {
        PurgeComm(hCom, PURGE_RXCLEAR);//���ͨ�Ŷ˿�����
    }
    while (IsStop == FALSE)
    {
        memset(str, '\0', sizeof(str));//�ڴ��ʼ��

        unsigned char framehead1[1], framehead2[1]; //֡ͷ0xA5,0x5A ֡β0xB6,0x6B
        unsigned char framehead3[4]; //ʶ����0x02,������0x02,���ݸ���0x00C0
        bReadStat = ReadFile(hCom, framehead1, hdrsize, &hdrsize, NULL);//Ѱ��֡ͷ
        if (framehead1[0] != 0xA5) //У��֡ͷ
        {
            continue;
        }
        bReadStat = ReadFile(hCom, framehead2, hdrsize, &hdrsize, NULL);
        if (framehead2[0] != 0x5A) //У��֡ͷ
        {
            continue;
        }
        bReadStat = ReadFile(hCom, framehead3, hdrsize1, &hdrsize1, NULL);
        if (framehead3[0] != 0x02 && framehead3[1] != 0x02 && framehead3[2] != 0x00 && framehead3[3] != 0xC0) //ʶ����,������,���ݸ���
        {
            continue;
        }
        bReadStat = ReadFile(hCom, str, readsize, &readsize, NULL);
        if (str[194] != 0xB6 || str[195] != 0x6B)//У��֡β
        {
            continue;
        }

        if (CH != 0)
        {
            double pos[6];//�ֱ���[X,Y,Z,PHI,THETA,GAMMA]
            switch (CH)
            {
            case 1:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 38]) + (str[i * 4 + 1 + 38] << 8) + (str[i * 4 + 2 + 38] << 16)
                        + (str[i * 4 + 3 + 38] << 24)) / 10000.0;
                    cout << pos[i] << " ";//����������
                }
                cout << endl;
                break;
            case 2:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 64]) + (str[i * 4 + 1 + 64] << 8) + (str[i * 4 + 2 + 64] << 16)
                        + (str[i * 4 + 3 + 64] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 3:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 90]) + (str[i * 4 + 1 + 90] << 8) + (str[i * 4 + 2 + 90] << 16)
                        + (str[i * 4 + 3 + 90] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 4:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 116]) + (str[i * 4 + 1 + 116] << 8) + (str[i * 4 + 2 + 116] << 16)
                        + (str[i * 4 + 3 + 116] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            case 5:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 142]) + (str[i * 4 + 1 + 142] << 8) + (str[i * 4 + 2 + 142] << 16)
                        + (str[i * 4 + 3 + 142] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << endl;
                break;
            default:
                break;
            }

            std::vector<TrackingData> trackingdatas;

            TrackingData DataTemp;
            DataTemp.POSITION[0] = pos[0];
            DataTemp.POSITION[1] = pos[1];
            DataTemp.POSITION[2] = pos[2];
            DataTemp.POSITION[3] = pos[3];
            DataTemp.POSITION[4] = pos[4];
            DataTemp.POSITION[5] = pos[5];
            trackingdatas.push_back(DataTemp);

            //vtkPolyData* polydatatemp = vtkPolyData::New();
            emit UpdateDataSignal(trackingdatas);
            //break;//ɾ���������һֱȡ��
        }
        else
        {
            double pos[5][6];//�ֱ���[X,Y,Z,PHI,THETA,GAMMA]
            for (int i = 0; i < 6; i++)
            {
                pos[0][i] = ((str[i * 4 + 38]) + (str[i * 4 + 1 + 38] << 8) + (str[i * 4 + 2 + 38] << 16)
                    + (str[i * 4 + 3 + 38] << 24)) / 10000.0;
                pos[1][i] = ((str[i * 4 + 64]) + (str[i * 4 + 1 + 64] << 8) + (str[i * 4 + 2 + 64] << 16)
                    + (str[i * 4 + 3 + 64] << 24)) / 10000.0;
                pos[2][i] = ((str[i * 4 + 90]) + (str[i * 4 + 1 + 90] << 8) + (str[i * 4 + 2 + 90] << 16)
                    + (str[i * 4 + 3 + 90] << 24)) / 10000.0;
                pos[3][i] = ((str[i * 4 + 116]) + (str[i * 4 + 1 + 116] << 8) + (str[i * 4 + 2 + 116] << 16)
                    + (str[i * 4 + 3 + 116] << 24)) / 10000.0;
                pos[4][i] = ((str[i * 4 + 142]) + (str[i * 4 + 1 + 142] << 8) + (str[i * 4 + 2 + 142] << 16)
                    + (str[i * 4 + 3 + 142] << 24)) / 10000.0;
            }

            std::vector<TrackingData> trackingdatas;
            TrackingData DataTemp;
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    DataTemp.POSITIONALL[i][j] = pos[i][j];
                }
            }

            //std::cout << datatemp.POSITIONALL[0][0] << " " << datatemp.POSITIONALL[0][1] << " " << datatemp.POSITIONALL[0][2] << " " << datatemp.POSITIONALL[0][3] << " " << datatemp.POSITIONALL[0][4] << " " << datatemp.POSITIONALL[0][5] << endl;
            //std::cout << datatemp.POSITIONALL[1][0] << " " << datatemp.POSITIONALL[1][1] << " " << datatemp.POSITIONALL[1][2] << " " << datatemp.POSITIONALL[1][3] << " " << datatemp.POSITIONALL[1][4] << " " << datatemp.POSITIONALL[1][5] << endl;
            //std::cout << datatemp.POSITIONALL[2][0] << " " << datatemp.POSITIONALL[2][1] << " " << datatemp.POSITIONALL[2][2] << " " << datatemp.POSITIONALL[2][3] << " " << datatemp.POSITIONALL[2][4] << " " << datatemp.POSITIONALL[2][5] << endl;
            //std::cout << datatemp.POSITIONALL[3][0] << " " << datatemp.POSITIONALL[3][1] << " " << datatemp.POSITIONALL[3][2] << " " << datatemp.POSITIONALL[3][3] << " " << datatemp.POSITIONALL[3][4] << " " << datatemp.POSITIONALL[3][5] << endl;
            //std::cout << datatemp.POSITIONALL[4][0] << " " << datatemp.POSITIONALL[4][1] << " " << datatemp.POSITIONALL[4][2] << " " << datatemp.POSITIONALL[4][3] << " " << datatemp.POSITIONALL[4][4] << " " << datatemp.POSITIONALL[4][5] << endl;
            //std::cout << std::endl;

            trackingdatas.push_back(DataTemp);

            //vtkPolyData* polydatatemp = vtkPolyData::New();
            emit UpdateDataSignal(trackingdatas);
            //break;//ɾ���������һֱȡ��
        }
        if (IsStop == TRUE)
        {
            break;
        }
    }
}*/
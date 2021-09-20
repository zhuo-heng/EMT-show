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
}

TrackWork::~TrackWork()
{

}

// �̴߳�����(���ݲ�ͬ�Ĺ���ģʽ���ö�Ӧ�Ĵ�����)
void TrackWork::ThreadWorkFunction()
{
    switch (m_WorkMode)
    {
    case EMTTrackCH1:
        this->GetData(1);
        break;
    case EMTTrackCH2:
        this->GetData(2);
        break;
    case EMTTrackCH3:
        this->GetData(3);
        break;
    case EMTTrackCH4:
        this->GetData(4);
        break;
    case EMTTrackCH5:
        this->GetData(5);
        break;
    case EMTTrackCHALL:
        this->GetData(0);
        break;
    default:
        break;
    }
}

// ���ù���ģʽ
void TrackWork::SetWorkMode(WorkMode w)
{
    m_WorkMode = w;
}


// ��ȡ��������
void TrackWork::GetData(int CH)
{
    OpenPort();
    ConfigPort();
    WPort();
    RPort(CH);
    ClosePort();
}

// �����̹߳رձ�־λ
void TrackWork::SetStopFlag(BOOL flag)
{
    IsStop = flag;
}

void TrackWork::WPort()
{
    unsigned char lpOutBuffer[11];
    lpOutBuffer[0] = 0xA5;	  //�������ݽ���ָ��
    lpOutBuffer[1] = 0x5A;
    lpOutBuffer[2] = 0x01;
    lpOutBuffer[3] = 0x01;
    lpOutBuffer[4] = 0x00;
    lpOutBuffer[5] = 0x01;
    lpOutBuffer[6] = 0x00;
    lpOutBuffer[7] = 0xE1;
    lpOutBuffer[8] = 0xF0;
    lpOutBuffer[9] = 0xB6;
    lpOutBuffer[10] = 0x6B;

    DWORD dwBytesWrite = 11;//Ҫд����ֽ���
    COMSTAT ComStat;
    DWORD dwErrorFlags;
    BOOL bWriteStat;
    ClearCommError(hCom1, &dwErrorFlags, &ComStat);
    bWriteStat = WriteFile(hCom1, lpOutBuffer, dwBytesWrite, &dwBytesWrite, NULL);

    if (!bWriteStat)
    {
        cout << "д����ʧ��" << std::endl;
    }
    else
    {
        cout << "д���ڳɹ�" << std::endl;
    }

    PurgeComm(hCom1, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);//����������������
}

void TrackWork::RPort(int CH)
{
    unsigned char str[196];
    DWORD readsize = 196;//��ȡ���ֽ���
    DWORD dwError;
    DWORD hdrsize = 1;
    DWORD hdrsize1 = 4;
    BOOL bReadStat;
    COMSTAT cs;
    WaitForSingleObject(hCom1, INFINITE);
    ClearCommError(hCom1, &dwError, &cs);//ȡ��״̬
    //�ж������Ƿ������׼��������
    if (cs.cbInQue > sizeof(str))
    {
        PurgeComm(hCom1, PURGE_RXCLEAR);//���ͨ�Ŷ˿�����
    }
    int i = 1;
    while (i++ && isCapture)
    {
        //std::cout << i << endl;
        memset(str, '\0', sizeof(str));//�ڴ��ʼ��

        unsigned char framehead1[1], framehead2[1]; //֡ͷ0xA5,0x5A ֡β0xB6,0x6B
        unsigned char framehead3[4]; //ʶ����0x02,������0x02,���ݸ���0x00C0
        bReadStat = ReadFile(hCom1, framehead1, hdrsize, &hdrsize, NULL);//Ѱ��֡ͷ
        //std::cout << std::hex << framehead1[0] / 16 << " ";
        //std::cout << framehead1[0] % 16 << std::endl;
        if (framehead1[0] != 0xA5) //У��֡ͷ
        {
            continue;
        }
        bReadStat = ReadFile(hCom1, framehead2, hdrsize, &hdrsize, NULL);
        if (framehead2[0] != 0x5A) //У��֡ͷ
        {
            continue;
        }
        bReadStat = ReadFile(hCom1, framehead3, hdrsize1, &hdrsize1, NULL);
        if (framehead3[0] != 0x02 && framehead3[1] != 0x02 && framehead3[2] != 0x00 && framehead3[3] != 0xC0) //ʶ����,������,���ݸ���
        {
            continue;
        }
        bReadStat = ReadFile(hCom1, str, readsize, &readsize, NULL);
        if (str[194] != 0xB6 || str[195] != 0x6B)//У��֡β
        {
            continue;
            //std::cout << "������ʧ��" << std::endl;
        }

        if (CH != 12345)
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
                cout << std::endl;
                break;
            case 2:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 64]) + (str[i * 4 + 1 + 64] << 8) + (str[i * 4 + 2 + 64] << 16)
                        + (str[i * 4 + 3 + 64] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << std::endl;
                break;
            case 3:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 90]) + (str[i * 4 + 1 + 90] << 8) + (str[i * 4 + 2 + 90] << 16)
                        + (str[i * 4 + 3 + 90] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << std::endl;
                break;
            case 4:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 116]) + (str[i * 4 + 1 + 116] << 8) + (str[i * 4 + 2 + 116] << 16)
                        + (str[i * 4 + 3 + 116] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << std::endl;
                break;
            case 5:
                for (int i = 0; i < 6; i++)
                {
                    pos[i] = ((str[i * 4 + 142]) + (str[i * 4 + 1 + 142] << 8) + (str[i * 4 + 2 + 142] << 16)
                        + (str[i * 4 + 3 + 142] << 24)) / 10000.0;
                    cout << pos[i] << " ";
                }
                cout << std::endl;
                break;
            default:
                break;
            }

            std::vector<TrackingData> trackingdatas;

            TrackingData datatemp;
            datatemp.POSITION[0] = pos[0];
            datatemp.POSITION[1] = pos[1];
            datatemp.POSITION[2] = pos[2];
            datatemp.POSITION[3] = pos[3];
            datatemp.POSITION[4] = pos[4];
            datatemp.POSITION[5] = pos[5];
            trackingdatas.push_back(datatemp);

            vtkPolyData* polydatatemp = vtkPolyData::New();
            emit SignalShowTrackingData(polydatatemp, trackingdatas);
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
            TrackingData datatemp;
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    datatemp.POSITIONALL[i][j] = pos[i][j];
                }
            }

            /*std::cout << datatemp.POSITIONALL[0][0] << " " << datatemp.POSITIONALL[0][1] << " " << datatemp.POSITIONALL[0][2] << " " << datatemp.POSITIONALL[0][3] << " " << datatemp.POSITIONALL[0][4] << " " << datatemp.POSITIONALL[0][5] << endl;
            std::cout << datatemp.POSITIONALL[1][0] << " " << datatemp.POSITIONALL[1][1] << " " << datatemp.POSITIONALL[1][2] << " " << datatemp.POSITIONALL[1][3] << " " << datatemp.POSITIONALL[1][4] << " " << datatemp.POSITIONALL[1][5] << endl;
            std::cout << datatemp.POSITIONALL[2][0] << " " << datatemp.POSITIONALL[2][1] << " " << datatemp.POSITIONALL[2][2] << " " << datatemp.POSITIONALL[2][3] << " " << datatemp.POSITIONALL[2][4] << " " << datatemp.POSITIONALL[2][5] << endl;
            std::cout << datatemp.POSITIONALL[3][0] << " " << datatemp.POSITIONALL[3][1] << " " << datatemp.POSITIONALL[3][2] << " " << datatemp.POSITIONALL[3][3] << " " << datatemp.POSITIONALL[3][4] << " " << datatemp.POSITIONALL[3][5] << endl;
            std::cout << datatemp.POSITIONALL[4][0] << " " << datatemp.POSITIONALL[4][1] << " " << datatemp.POSITIONALL[4][2] << " " << datatemp.POSITIONALL[4][3] << " " << datatemp.POSITIONALL[4][4] << " " << datatemp.POSITIONALL[4][5] << endl;
            std::cout << std::endl;*/

            trackingdatas.push_back(datatemp);

            vtkPolyData* polydatatemp = vtkPolyData::New();
            emit SignalShowTrackingData(polydatatemp, trackingdatas);
            //break;//ɾ���������һֱȡ��
        }
    }
}
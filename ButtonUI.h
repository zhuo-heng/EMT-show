#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QTimer>
#include <QColor>

class SwitchButton :
    public QWidget
{
    Q_OBJECT

public:
    explicit SwitchButton(QWidget* parent = nullptr);
    ~SwitchButton();

signals:
    void statusChanged(bool checked);   //�Ƿ�ʼ׷���ź�

private slots:
    void updateValue();

private:
    void drawBackGround(QPainter* painter);
    void drawSlider(QPainter* painter);

protected:
    void paintEvent(QPaintEvent* ev);
    void mousePressEvent(QMouseEvent* ev);

private:
    int m_space;                //�������߽����
    int m_radius;               //Բ�ǽǶ�

    bool m_checked;             //�Ƿ�ѡ��
    bool m_showText;            //�Ƿ���ʾ����
    bool m_showCircle;          //�Ƿ���ʾԲȦ
    bool m_animation;           //�Ƿ�ʹ�ö���

    QColor m_bgColorOn;         //��ʱ��ı���ɫ
    QColor m_bgColorOff;        //�ر�ʱ��ı���ɫ
    QColor m_sliderColorOn;     //��ʱ�򻬿���ɫ
    QColor m_sliderColorOff;    //�ر�ʱ�򻬿���ɫ

    QTimer* m_timer;            //������ʱ��
    int     m_step;             //��������
    int     m_startX;           //���鿪ʼX������
    int     m_endX;             //�������X������
};

#endif // SWITCHBUTTON_H
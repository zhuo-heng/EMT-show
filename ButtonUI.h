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
    void statusChanged(bool checked);   //是否开始追踪信号

private slots:
    void updateValue();

private:
    void drawBackGround(QPainter* painter);
    void drawSlider(QPainter* painter);

protected:
    void paintEvent(QPaintEvent* ev);
    void mousePressEvent(QMouseEvent* ev);

private:
    int m_space;                //滑块距离边界距离
    int m_radius;               //圆角角度

    bool m_checked;             //是否选中
    bool m_showText;            //是否显示文字
    bool m_showCircle;          //是否显示圆圈
    bool m_animation;           //是否使用动画

    QColor m_bgColorOn;         //打开时候的背景色
    QColor m_bgColorOff;        //关闭时候的背景色
    QColor m_sliderColorOn;     //打开时候滑块颜色
    QColor m_sliderColorOff;    //关闭时候滑块颜色

    QTimer* m_timer;            //动画定时器
    int     m_step;             //动画步长
    int     m_startX;           //滑块开始X轴坐标
    int     m_endX;             //滑块结束X轴坐标
};

#endif // SWITCHBUTTON_H
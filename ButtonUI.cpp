/***********************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
// 
// function : ButtonUI.cpp
// 
// 负责“选择”按钮的动画效果
/***********************************************************************************************/
#include "ButtonUI.h"
#include <QPainter>

SwitchButton::SwitchButton(QWidget* parent) 
	: QWidget(parent)
{
    m_space = 2;
    m_radius = 5;
    m_checked = false;
    m_showText = true;
    m_showText = false;
    m_animation = true;

    m_bgColorOn = QColor(84, 187, 255);
    m_bgColorOff = QColor(111, 122, 126);

    m_sliderColorOn = QColor(84, 187, 255);
    m_sliderColorOff = QColor(255, 255, 255);

    m_step = 0;
    m_startX = 0;
    m_endX = 0;

    m_timer = new QTimer(this);
    m_timer->setInterval(30);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateValue()));
}

SwitchButton::~SwitchButton()
{

}


void SwitchButton::drawBackGround(QPainter* painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    QColor bgColor = m_checked ? m_bgColorOn : m_bgColorOff;
    if (isEnabled()) {
        bgColor.setAlpha(60);
    }

    painter->setBrush(bgColor);

    QRect rect(0, 0, width(), height());
    int side = qMin(width(), height());

    //左侧半圆
    QPainterPath path1;
    path1.addEllipse(rect.x(), rect.y(), side, side);

    //右侧半圆
    QPainterPath path2;
    path2.addEllipse(rect.width() - side, rect.y(), side, side);

    //中间的矩形
    QPainterPath path3;
    path3.addRect(rect.x() + side / 2, rect.y(), rect.width() - side, height());

    QPainterPath path = path1 + path2 + path3;
    painter->drawPath(path);

    painter->restore();
}


void SwitchButton::drawSlider(QPainter* painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    QColor color = m_checked ? m_sliderColorOn : m_sliderColorOff;

    painter->setBrush(QBrush(color));

    int sliderWidth = qMin(width(), height()) - m_space * 2;
    QRect rect(m_space + m_startX, m_space, sliderWidth, sliderWidth);
    painter->drawEllipse(rect);

    painter->restore();
}


void SwitchButton::paintEvent(QPaintEvent* ev)
{
    //启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景
    drawBackGround(&painter);

    //绘制滑块
    drawSlider(&painter);
}


void SwitchButton::mousePressEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev)

        m_checked = !m_checked;
    emit statusChanged(m_checked);

    //计算步长
    m_step = width() / 10;

    //计算滑块X轴终点坐标
    if (m_checked) {
        m_endX = width() - height();
    }
    else {
        m_endX = 0;
    }

    //判断是否使用动画
    if (m_animation) {
        m_timer->start();
    }
    else {
        m_startX = m_endX;
        update();
    }
}


void SwitchButton::updateValue()
{
    if (m_checked) {
        if (m_startX < m_endX) {
            m_startX += m_step;
        }
        else {
            m_startX = m_endX;
            m_timer->stop();
        }
    }
    else {
        if (m_startX > m_endX) {
            m_startX -= m_step;
        }
        else {
            m_startX = m_endX;
            m_timer->stop();
        }
    }
    update();
}
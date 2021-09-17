/***********************************************************************************************/
// Copyright (c) 2021, zhuo-heng
// All rights reserved.
// This code is licensed under the BSD 3-Clause License.
// 
// function : ButtonUI.cpp
// 
// ����ѡ�񡱰�ť�Ķ���Ч��
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

    //����Բ
    QPainterPath path1;
    path1.addEllipse(rect.x(), rect.y(), side, side);

    //�Ҳ��Բ
    QPainterPath path2;
    path2.addEllipse(rect.width() - side, rect.y(), side, side);

    //�м�ľ���
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
    //���÷����
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //���Ʊ���
    drawBackGround(&painter);

    //���ƻ���
    drawSlider(&painter);
}


void SwitchButton::mousePressEvent(QMouseEvent* ev)
{
    Q_UNUSED(ev)

        m_checked = !m_checked;
    emit statusChanged(m_checked);

    //���㲽��
    m_step = width() / 10;

    //���㻬��X���յ�����
    if (m_checked) {
        m_endX = width() - height();
    }
    else {
        m_endX = 0;
    }

    //�ж��Ƿ�ʹ�ö���
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
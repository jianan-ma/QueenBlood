#include "cellwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
CellWidget::CellWidget(CellUnit* unit, QWidget *parent) 
    : QWidget(parent), m_cellUnit(unit), m_isHighlighted(false), m_isHovered(false)
{
    setMinimumSize(80, 80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CellWidget::setCellUnit(CellUnit* unit)
{
    m_cellUnit = unit;
    update();
}

CellUnit* CellWidget::getCellUnit() const
{
    return m_cellUnit;
}

void CellWidget::highlight(bool enabled)
{
    m_isHighlighted = enabled;
    update();
}

void CellWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制单元格背景
    QRect rect = contentsRect();
    
    // 根据阵营绘制底色
    switch(m_cellUnit->getCampArea()) {
    case CAMP::CAMP_RED:
        painter.fillRect(rect, QColor(255, 200, 200));
        break;
    case CAMP::CAMP_BLUE:
        painter.fillRect(rect, QColor(200, 200, 255));
        break;
    default:
        painter.fillRect(rect, QColor(230, 230, 230));
    }
    
    // 绘制边框
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(rect.adjusted(0, 0, -1, -1));

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(rect.adjusted(5, 5, -5, -30),
                    Qt::AlignCenter,
                    QString("Lv.%1").arg(m_cellUnit->getCampLevel()));
    
    // 如果有卡牌，绘制卡牌信息
    if(m_cellUnit->getCampCard() != CAMP::CAMP_NULL) {
        // 绘制卡牌阵营标识
        QColor campColor = (m_cellUnit->getCampCard() == CAMP::CAMP_RED) ? 
                           QColor(255, 100, 100) : QColor(100, 100, 255);
        painter.fillRect(rect.adjusted(5, 5, -5, -30), campColor);
        
        // 绘制等级和分数

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(20,20,m_cellUnit->getCardName());
        painter.drawText(rect.adjusted(5, rect.height()-25, -5, -5), 
                        Qt::AlignCenter, 
                        QString("★%1").arg(m_cellUnit->getScore()));
    }
    
    // 高亮效果
    if(m_isHighlighted) {
        painter.fillRect(rect, QColor(255, 255, 0, 50));
    }
    
    // 悬停效果
    if(m_isHovered) {
        painter.setPen(QPen(Qt::blue, 3));
        painter.drawRect(rect.adjusted(0, 0, -1, -1));
    }
}

void CellWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit clicked(m_cellUnit);
}

void CellWidget::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    m_isHovered = true;
    update();
    emit hovered(m_cellUnit, true);
}

void CellWidget::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    m_isHovered = false;
    update();
    emit hovered(m_cellUnit, false);
}

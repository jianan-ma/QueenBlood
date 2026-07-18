#include "cardwidget.h"
#include <QPainter>
#include <QMouseEvent>

const int CARD_WIDTH = 80;
const int CARD_HEIGHT = 100;

int CardWidget::cardWidth() { return CARD_WIDTH; }
int CardWidget::cardHeight() { return CARD_HEIGHT; }

CardWidget::CardWidget(Card* card, QWidget *parent) 
    : QWidget(parent), m_card(card), m_isSelected(false), m_isHovered(false)
{
    setFixedSize(CARD_WIDTH, CARD_HEIGHT);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

Card* CardWidget::getCard() const
{
    return m_card;
}

void CardWidget::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

bool CardWidget::isSelected() const
{
    return m_isSelected;
}

void CardWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect rect = contentsRect();
    
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, QColor(245, 245, 250));
    gradient.setColorAt(1, QColor(200, 200, 220));
    painter.fillRect(rect, gradient);
    
    if(m_isSelected) {
        painter.setPen(QPen(QColor(100, 150, 255), 6));
    } else {
        painter.setPen(QPen(Qt::darkGray, 2));
    }
    painter.drawRoundedRect(rect, 8, 8);
    
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRoundedRect(rect, 6, 6);
    
    QColor costColor;
    if(m_card->getCost() == 1) costColor = QColor(100, 180, 100);
    else if(m_card->getCost() == 2) costColor = QColor(100, 100, 200);
    else costColor = QColor(200, 100, 100);
    
    QRect costRect = QRect(rect.left() + 4, rect.top() + 4, 24, 24);
    painter.setBrush(costColor);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(costRect);
    
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.drawText(costRect, Qt::AlignCenter, QString::number(m_card->getCost()));
    
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    QString name = m_card->getName();
    if(name.length() > 6) {
        name = name.left(5) + "...";
    }
    painter.drawText(QRect(rect.left() + 30, rect.top() + 8, 
                           rect.width() - 35, 20), 
                     Qt::AlignLeft | Qt::AlignVCenter, name);
    
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(QColor(200, 80, 80));
    painter.drawText(QRect(rect.left() + 8, rect.top() + 35, 
                           rect.width() - 16, 20), 
                     Qt::AlignCenter, QString("★%1").arg(m_card->getPower()));
    
    QString typeText = m_card->getType() == TYPE::NORMAL ? "普通" : "覆盖";
    painter.setFont(QFont("Arial", 8));
    painter.setPen(Qt::gray);
    painter.drawText(QRect(rect.left() + 8, rect.bottom() - 15, 
                           rect.width() - 16, 12), 
                     Qt::AlignCenter, typeText);
    
    if(m_isHovered) {
        painter.fillRect(rect, QColor(255, 255, 255, 60));
    }
}

void CardWidget::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit clicked(m_card);
}

void CardWidget::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    m_isHovered = true;
    update();
}

void CardWidget::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    m_isHovered = false;
    update();
}

#include "cardhandpanel.h"

CardHandPanel::CardHandPanel(QWidget *parent) 
    : QWidget(parent), m_selectedWidget(nullptr)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(5);
    m_layout->setContentsMargins(5, 5, 5, 5);
    setMinimumHeight(CardWidget::cardHeight() + 10);
}

CardHandPanel::~CardHandPanel()
{
    clearCards();
}

void CardHandPanel::addCard(Card* card)
{
    CardWidget* widget = new CardWidget(card, this);
    m_cardWidgets.append(widget);
    m_layout->addWidget(widget);
    
    connect(widget, &CardWidget::clicked, this, [this, widget](Card* card) {
        if(m_selectedWidget == widget) {
            widget->setSelected(false);
            m_selectedWidget = nullptr;
            emit cardDeselected();
        } else {
            if(m_selectedWidget) {
                m_selectedWidget->setSelected(false);
            }
            m_selectedWidget = widget;
            widget->setSelected(true);
            emit cardSelected(card);
        }
    });
}

void CardHandPanel::removeCard(Card* card)
{
    for(int i = 0; i < m_cardWidgets.size(); ++i) {
        if(m_cardWidgets[i]->getCard() == card) {
            CardWidget* widget = m_cardWidgets.takeAt(i);
            if(m_selectedWidget == widget) {
                m_selectedWidget = nullptr;
            }
            m_layout->removeWidget(widget);
            delete widget;
            return;
        }
    }
}

void CardHandPanel::clearCards()
{
    qDeleteAll(m_cardWidgets);
    m_cardWidgets.clear();
    m_selectedWidget = nullptr;
    emit cardDeselected();
}

void CardHandPanel::selectCard(Card* card)
{
    for(CardWidget* widget : m_cardWidgets) {
        if(widget->getCard() == card) {
            if(m_selectedWidget) {
                m_selectedWidget->setSelected(false);
            }
            m_selectedWidget = widget;
            widget->setSelected(true);
            return;
        }
    }
}

Card* CardHandPanel::getSelectedCard() const
{
    return m_selectedWidget ? m_selectedWidget->getCard() : nullptr;
}

QVector<Card*> CardHandPanel::getCards() const
{
    QVector<Card*> cards;
    for(CardWidget* widget : m_cardWidgets) {
        cards.append(widget->getCard());
    }
    return cards;
}

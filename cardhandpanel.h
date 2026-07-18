#ifndef CARDHANDPANEL_H
#define CARDHANDPANEL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVector>
#include "cardwidget.h"
#include "cards/card.h"

class CardHandPanel : public QWidget
{
    Q_OBJECT
public:
    explicit CardHandPanel(QWidget *parent = nullptr);
    ~CardHandPanel();
    
    void addCard(Card* card);
    void removeCard(Card* card);
    void clearCards();
    void selectCard(Card* card);
    Card* getSelectedCard() const;
    QVector<Card*> getCards() const;

private:
    QHBoxLayout* m_layout;
    QVector<CardWidget*> m_cardWidgets;
    CardWidget* m_selectedWidget;
    
signals:
    void cardSelected(Card* card);
    void cardDeselected();
};

#endif // CARDHANDPANEL_H

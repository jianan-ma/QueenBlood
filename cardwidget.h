#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include "cards/card.h"

class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(Card* card, QWidget *parent = nullptr);
    
    Card* getCard() const;
    void setSelected(bool selected);
    bool isSelected() const;
    
    static int cardWidth();
    static int cardHeight();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    
private:
    Card* m_card;
    bool m_isSelected;
    bool m_isHovered;
    
signals:
    void clicked(Card* card);
};

#endif // CARDWIDGET_H
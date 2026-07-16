#ifndef CELLWIDGET_H
#define CELLWIDGET_H

#include <QWidget>
#include "cellunit.h"

class CellWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CellWidget(CellUnit* unit, QWidget *parent = nullptr);
    void setCellUnit(CellUnit* unit);
    CellUnit* getCellUnit() const;
    void highlight(bool enabled);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    
private:
    CellUnit* m_cellUnit;
    bool m_isHighlighted;
    bool m_isHovered;
    
signals:
    void clicked(CellUnit* unit);
    void hovered(CellUnit* unit, bool entered);
};

#endif // CELLWIDGET_H
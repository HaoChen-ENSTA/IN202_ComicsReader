#ifndef CBRSLIDER_H
#define CBRSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QLabel>

class CbrSlider : public QSlider
{
//public:
//    CbrSlider();
public:
    CbrSlider(QWidget *parent=0);
    ~CbrSlider();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    QLabel* m_displayLabel;
};

#endif // CBRSLIDER_H

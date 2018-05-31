#ifndef CBRGRAPHICSVIEW_H
#define CBRGRAPHICSVIEW_H
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{

public:
    GraphicsView(QWidget* parent = 0);
    int getFactor();
    void setFactor(int fact);
//    void paintEvent(QPaintEvent *);
private:
    int factor;
};

#endif // CBRGRAPHICSVIEW_H

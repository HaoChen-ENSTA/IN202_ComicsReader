#include "graphicsview.hpp"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    factor = 0;
}
void GraphicsView::setFactor(int fact) {

    if (fact == 0)
        factor = 0;
    else
        factor += fact;
}
int GraphicsView::getFactor() { return factor; }

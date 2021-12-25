#ifndef WATERFALL_H
#define WATERFALL_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QColor>
#include <QImage>
#include <QList>
#include <QRgb>
#include <iostream>

// Inspired by: 

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT

public:
    Waterfall(QQuickItem *parent = 0);

    void paint(QPainter *painter);

public slots:
    void addSamples(std::vector<float> samples);

private:
    QImage image;
    QList<QRgb> colors;
    uint64_t ignoreCounter;

private slots:
    void sizeChanged();

};

#endif
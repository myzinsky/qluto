#ifndef WATERFALL_H
#define WATERFALL_H

#include <QtQuick/QQuickPaintedItem>
#include <QPainter>
#include <QColor>
#include <QImage>
#include <QList>
#include <QRgb>
#include <iostream>
#include "dsp.h"

// Inspired by: 

class Waterfall : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(fft* dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)

public:
    Waterfall(QQuickItem *parent = 0);

    void paint(QPainter *painter);

    fft *dataSource() const;
    void setDataSource(fft *value);

private:
    void addSamples(std::vector<float> samples);

private:
    QImage image;
    QList<QRgb> colors;
    uint64_t ignoreCounter;
    fft *m_dataSource = nullptr;

private slots:
    void sizeChanged();

Q_SIGNALS:
    void dataSourceChanged();

};

#endif

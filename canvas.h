#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QTransform>
#include <QWidget>
#include <QRectF>
#include "objetografico.h"

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);
    void definirDisplayFile(const QVector<ObjetoGrafico> *df);
    void definirMundo(const QRectF &w);
    void definirTransformacao(const QTransform &t);

signals:
    void viewportAlterado(const QRectF &viewport);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool clipReta(QPointF &p1, QPointF &p2) const;

    const QVector<ObjetoGrafico> *m_displayFile = nullptr;
    QTransform m_transformacao;
    QRectF m_mundo;
    QRect m_viewportRect;
};

#endif // CANVAS_H

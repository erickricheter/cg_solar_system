#ifndef CANVAS_H
#define CANVAS_H

#include <QPainter>
#include <QTransform>
#include <QWidget>
#include <QRectF>
#include <QMatrix4x4>      // MUDANÇA: Incluir matriz 3D
#include "objeto3d.h"       // MUDANÇA: Incluir classe 3D

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QWidget *parent = nullptr);

    // MUDANÇA: Funções 2D removidas/alteradas
    void definirDisplayFile3D(const QVector<Objeto3D*> *df);
    void definirTransformacao3D(const QMatrix4x4 &vpMatrix, const QRectF &viewport);

signals:
    void viewportAlterado(const QRectF &viewport);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; // Esta função é ótima, vamos mantê-la!

private:
    // MUDANÇA: Função de clipping 2D removida (será feita em 3D futuramente)
    // bool clipReta(QPointF &p1, QPointF &p2) const;

    // MUDANÇA: Função auxiliar para o pipeline 3D
    QPointF transformarPonto(const QVector3D& pontoNdc);

    // MUDANÇA: Armazenamento de dados 3D
    const QVector<Objeto3D*> *m_displayFile3D = nullptr;
    QMatrix4x4 m_vpMatrix; // Matriz View-Projection
    QRectF m_viewport;     // A viewport 2D (controlada pelo resizeEvent)

    // MUDANÇA: m_mundo e m_transformacao 2D não são mais necessários aqui
    // QRectF m_mundo;
    // QTransform m_transformacao;
    QRect m_viewportRect; // Esta é usada pelo seu resizeEvent, mantenha!
};

#endif // CANVAS_H


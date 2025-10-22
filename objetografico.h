#ifndef OBJETOGRAFICO_H
#define OBJETOGRAFICO_H

#include <QString>
#include <QVector>
#include <QPointF>
#include <QColor>

enum TipoObjeto {
    PONTO,
    RETA,
    POLIGONO
};

class ObjetoGrafico {
public:
    QString nome;
    TipoObjeto tipo;
    QVector<QPointF> pontos;
    QColor cor;

    ObjetoGrafico() = default;

    ObjetoGrafico(QString n, TipoObjeto t, const QVector<QPointF>& p, const QColor& c = Qt::black)
        : nome(n), tipo(t), pontos(p), cor(c) {}
};

#endif // OBJETOGRAFICO_H

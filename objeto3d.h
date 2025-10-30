#ifndef OBJETO3D_H
#define OBJETO3D_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor> // MUDANÇA: Incluir a classe de cor

// ... (struct Face continua a mesma) ...
struct Face
{
    int v1_idx, v2_idx, v3_idx;
};

class Objeto3D
{
public:
    Objeto3D(const QString& nome) : m_nome(nome), m_color(Qt::white) { // MUDANÇA: Iniciar cor
        m_modelMatrix.setToIdentity();
    }

    QString m_nome;
    QVector<QVector3D> m_vertices;
    QVector<Face> m_faces;
    QMatrix4x4 m_modelMatrix;

    // MUDANÇA: Adicionar membro de cor
    QColor m_color;

    // MUDANÇA: Função para definir a cor
    void setColor(const QColor& color) {
        m_color = color;
    }
};

#endif // OBJETO3D_H

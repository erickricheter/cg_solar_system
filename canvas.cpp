#include "canvas.h"
#include <QResizeEvent>
#include <QDebug> // Para depuração

// MUDANÇA: O clipping 2D (Cohen-Sutherland) foi removido
// namespace { ... }

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

// MUDANÇA: Nova função para receber o display file 3D
void Canvas::definirDisplayFile3D(const QVector<Objeto3D*> *df)
{
    m_displayFile3D = df;
    update();
}

// MUDANÇA: Nova função para receber as matrizes 3D e a viewport 2D
void Canvas::definirTransformacao3D(const QMatrix4x4 &vpMatrix, const QRectF &viewport)
{
    m_vpMatrix = vpMatrix;
    m_viewport = viewport; // A MainWindow nos diz qual viewport usar
    update();
}

// MUDANÇA: Esta é a função que transforma de Coordenadas 3D (NDC) para 2D (Tela)
QPointF Canvas::transformarPonto(const QVector3D& pontoNdc)
{
    // Coordenadas de Dispositivo Normalizadas (NDC) vão de -1 a 1.
    // Precisamos mapeá-las para as coordenadas de pixel da nossa viewport.

    // 1. Mapeia X: (ndc.x + 1) / 2 -> de [0, 1] | * largura_viewport + x_viewport
    double x = (pontoNdc.x() + 1.0) * 0.5 * m_viewport.width() + m_viewport.left();

    // 2. Mapeia Y: O eixo Y do Qt é invertido (0 é no topo), por isso (1.0 - ndc.y)
    double y = (1.0 - pontoNdc.y()) * 0.5 * m_viewport.height() + m_viewport.top();

    return QPointF(x, y);
}

// --- MUDANÇA: O NOVO PAINT EVENT 3D ---
void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!m_displayFile3D) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Desenha a borda da viewport (como você já fazia)
    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    painter.drawRect(m_viewportRect);

    // O Painter não deve ter nenhuma transformação 2D antiga
    painter.resetTransform();

    painter.setPen(QPen(Qt::blue, 1)); // Desenhar em "wireframe" azul
    painter.setBrush(Qt::NoBrush);

    // --- O PIPELINE DE RENDERIZAÇÃO 3D ---
    for (const Objeto3D* objeto : *m_displayFile3D)
    {
        // 1. Pega a matriz de modelo (posição/rotação/escala) do objeto
        const QMatrix4x4& modelMatrix = objeto->m_modelMatrix;

        // 2. Cria a matriz MVP final (Model-View-Projection) para este objeto
        QMatrix4x4 mvpMatrix = m_vpMatrix * modelMatrix;

        // 3. Itera sobre cada face (triângulo) do objeto
        for (const Face& face : objeto->m_faces)
        {
            // 4. Pega os vértices da face (em coordenadas de modelo/objeto)
            const QVector3D& v1_model = objeto->m_vertices[face.v1_idx];
            const QVector3D& v2_model = objeto->m_vertices[face.v2_idx];
            const QVector3D& v3_model = objeto->m_vertices[face.v3_idx];

            // 5. Transforma os vértices para Coordenadas de Dispositivo Normalizadas (NDC)
            //    usando a divisão de perspectiva (w)
            QVector3D v1_ndc = mvpMatrix.map(v1_model);
            QVector3D v2_ndc = mvpMatrix.map(v2_model);
            QVector3D v3_ndc = mvpMatrix.map(v3_model);

            // 6. Transforma de NDC para coordenadas de tela 2D (Viewport)
            QPointF p1 = transformarPonto(v1_ndc);
            QPointF p2 = transformarPonto(v2_ndc);
            QPointF p3 = transformarPonto(v3_ndc);

            // 7. Desenha as 3 arestas do triângulo (wireframe)
            painter.drawLine(p1, p2);
            painter.drawLine(p2, p3);
            painter.drawLine(p3, p1);
        }
    }
}

// --- Esta função é excelente e continua a mesma! ---
void Canvas::resizeEvent(QResizeEvent *event)
{
    int borda = 10;
    m_viewportRect.setX(borda);
    m_viewportRect.setY(borda);
    m_viewportRect.setWidth(event->size().width() - 2 * borda);
    m_viewportRect.setHeight(event->size().height() - 2 * borda);

    // Emite o sinal para a MainWindow saber a nova viewport
    emit viewportAlterado(QRectF(m_viewportRect));

    QWidget::resizeEvent(event);
}

// MUDANÇA: A função de clipping 2D foi removida
// bool Canvas::clipReta(...) { ... }


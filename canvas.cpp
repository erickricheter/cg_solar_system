#include "canvas.h"
#include <QResizeEvent>

namespace {
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000
}

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Canvas::definirDisplayFile(const QVector<ObjetoGrafico> *df)
{
    m_displayFile = df;
    update();
}

void Canvas::definirMundo(const QRectF &w)
{
    m_mundo = w;
}

void Canvas::definirTransformacao(const QTransform &t)
{
    m_transformacao = t;
    update();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!m_displayFile) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    painter.drawRect(m_viewportRect);

    painter.setTransform(m_transformacao);

    for (const auto &obj : *m_displayFile) {
        if (obj.pontos.isEmpty()) continue;

        painter.setPen(QPen(obj.cor, 0));
        painter.setBrush(QBrush(obj.cor));

        if (obj.tipo == PONTO) {
            if (m_mundo.contains(obj.pontos[0])) {
                painter.drawEllipse(obj.pontos[0], 1, 1);
            }
        }
        else if (obj.tipo == RETA) {
            if (obj.pontos.size() >= 2) {
                QPointF p1 = obj.pontos[0];
                QPointF p2 = obj.pontos[1];

                if (clipReta(p1, p2)) {
                    painter.setPen(QPen(obj.cor, 0));
                    painter.setBrush(Qt::NoBrush);
                    painter.drawLine(p1, p2);
                }
            }
        }
        else if (obj.tipo == POLIGONO) {
            if (obj.nome.startsWith("Orbita")) {
                painter.setPen(QPen(obj.cor, 0, Qt::DashLine));
                painter.setBrush(Qt::NoBrush);
            } else {
                painter.setPen(QPen(obj.cor, 0));
                painter.setBrush(QBrush(obj.cor));
            }
            painter.drawPolygon(QPolygonF(obj.pontos));
        }
    }
}

void Canvas::resizeEvent(QResizeEvent *event)
{
    int borda = 10;
    m_viewportRect.setX(borda);
    m_viewportRect.setY(borda);
    m_viewportRect.setWidth(event->size().width() - 2 * borda);
    m_viewportRect.setHeight(event->size().height() - 2 * borda);

    emit viewportAlterado(QRectF(m_viewportRect));

    QWidget::resizeEvent(event);
}

bool Canvas::clipReta(QPointF &p1, QPointF &p2) const
{
    auto computeOutCode = [&](const QPointF &p) {
        int code = INSIDE;
        if (p.x() < m_mundo.left()) code |= LEFT;
        else if (p.x() > m_mundo.right()) code |= RIGHT;
        if (p.y() < m_mundo.top()) code |= TOP;
        else if (p.y() > m_mundo.bottom()) code |= BOTTOM;
        return code;
    };

    int outcode1 = computeOutCode(p1);
    int outcode2 = computeOutCode(p2);
    bool accept = false;

    while (true) {
        if (!(outcode1 | outcode2)) {
            accept = true;
            break;
        } else if (outcode1 & outcode2) {
            break;
        } else {
            double x, y;
            int outcodeOut = outcode1 ? outcode1 : outcode2;

            if (outcodeOut & TOP) {
                x = p1.x() + (p2.x() - p1.x()) * (m_mundo.top() - p1.y()) / (p2.y() - p1.y());
                y = m_mundo.top();
            } else if (outcodeOut & BOTTOM) {
                x = p1.x() + (p2.x() - p1.x()) * (m_mundo.bottom() - p1.y()) / (p2.y() - p1.y());
                y = m_mundo.bottom();
            } else if (outcodeOut & RIGHT) {
                y = p1.y() + (p2.y() - p1.y()) * (m_mundo.right() - p1.x()) / (p2.x() - p1.x());
                x = m_mundo.right();
            } else {
                y = p1.y() + (p2.y() - p1.y()) * (m_mundo.left() - p1.x()) / (p2.x() - p1.x());
                x = m_mundo.left();
            }

            if (outcodeOut == outcode1) {
                p1.setX(x);
                p1.setY(y);
                outcode1 = computeOutCode(p1);
            } else {
                p2.setX(x);
                p2.setY(y);
                outcode2 = computeOutCode(p2);
            }
        }
    }
    return accept;
}

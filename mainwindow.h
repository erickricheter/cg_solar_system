#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QRectF>
#include <QDoubleSpinBox>
#include <QSignalBlocker>
#include "objetografico.h"

namespace Ui { class MainWindow; }
class Canvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void atualizarTransformacoes();
    void onViewportAlterado(const QRectF &viewport);
    void zoomIn();
    void zoomOut();

private:
    ObjetoGrafico criarPlaneta(const QString &nome, const QPointF &centro, double raio, int segmentos, const QColor &cor);
    ObjetoGrafico criarOrbita(const QString &nome, const QPointF &centro, double raio, int segmentos, const QColor &cor);

    void setupUiControls();
    void recalcularTransformacao();
    void atualizarUiPeloEstado();

    Ui::MainWindow *ui;
    Canvas *m_canvas;
    QVector<ObjetoGrafico> m_displayFile;

    QRectF m_window;
    QRectF m_viewport;

    QTransform m_transformacao;

    QDoubleSpinBox *winX, *winY, *winW, *winH;
};

#endif // MAINWINDOW_H

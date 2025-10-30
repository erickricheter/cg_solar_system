#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QRectF>
#include <QDoubleSpinBox>
#include <QSignalBlocker>
#include <QMatrix4x4>
#include "objeto3d.h"

// MUDANÇA: Incluir o QTimer
#include <QTimer>

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

    // MUDANÇA: Novo slot para o timer de animação
    void onAnimationTick();

private:
    bool carregarObjetoDeArquivo(const QString& caminho, Objeto3D* objeto);

    void setupUiControls();
    void recalcularTransformacao();
    void atualizarUiPeloEstado();

    Ui::MainWindow *ui;
    Canvas *m_canvas;
    QVector<Objeto3D*> m_displayFile;

    QRectF m_window;
    QRectF m_viewport;

    QMatrix4x4 m_viewProjectionMatrix;

    // MUDANÇA: Ponteiro para o timer
    QTimer* m_animationTimer;

    QDoubleSpinBox *winX, *winY, *winW, *winH;
};

#endif // MAINWINDOW_H


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvas.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <cmath>

namespace {
QDoubleSpinBox* createSpinBox(double min, double max, double value) {
    auto spin = new QDoubleSpinBox();
    spin->setRange(min, max);
    spin->setValue(value);
    spin->setSingleStep(10.0);
    return spin;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_viewport.setRect(0, 0, 100, 100);
    m_window.setRect(-500, -500, 1000, 1000);
    m_displayFile.append(criarPlaneta("Sol", QPointF(0, 0), 80, 24, Qt::yellow));
    m_displayFile.append(criarOrbita("Orbita Terra", QPointF(0, 0), 200, 48, Qt::lightGray));
    m_displayFile.append(criarPlaneta("Terra", QPointF(200, 0), 20, 16, Qt::blue));
    m_displayFile.append(criarOrbita("Orbita Marte", QPointF(0, 0), 350, 48, Qt::lightGray));
    m_displayFile.append(criarPlaneta("Marte", QPointF(350, 0), 15, 16, QColor(255, 100, 0)));

    m_displayFile.append(ObjetoGrafico("Reta Fora", RETA, { QPointF(-600, -600), QPointF(600, 600) }));


    m_canvas = new Canvas(this);
    m_canvas->definirDisplayFile(&m_displayFile);
    m_canvas->definirMundo(m_window);

    setupUiControls();
    recalcularTransformacao();
}

MainWindow::~MainWindow()
{
    delete ui;
}

ObjetoGrafico MainWindow::criarPlaneta(const QString &nome, const QPointF &centro, double raio, int segmentos, const QColor &cor)
{
    QVector<QPointF> pontos;
    for (int i = 0; i < segmentos; ++i) {
        double angulo = 2.0 * M_PI * i / segmentos;
        pontos.append(QPointF(centro.x() + raio * cos(angulo),
                              centro.y() + raio * sin(angulo)));
    }
    return ObjetoGrafico(nome, POLIGONO, pontos, cor);
}

ObjetoGrafico MainWindow::criarOrbita(const QString &nome, const QPointF &centro, double raio, int segmentos, const QColor &cor)
{
    return criarPlaneta(nome, centro, raio, segmentos, cor);
}

void MainWindow::setupUiControls()
{
    QWidget *centralWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    mainLayout->addWidget(m_canvas, 1);

    QWidget *controlPanel = new QWidget;
    controlPanel->setFixedWidth(250);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    auto windowGroup = new QGroupBox("Window (Mundo)");
    auto windowLayout = new QFormLayout(windowGroup);
    winX = createSpinBox(-5000, 5000, m_window.x());
    winY = createSpinBox(-5000, 5000, m_window.y());
    winW = createSpinBox(1, 10000, m_window.width());
    winH = createSpinBox(1, 10000, m_window.height());
    windowLayout->addRow("X min:", winX);
    windowLayout->addRow("Y min:", winY);
    windowLayout->addRow("Largura:", winW);
    windowLayout->addRow("Altura:", winH);

    auto zoomGroup = new QGroupBox("Controle de Zoom");
    auto zoomLayout = new QHBoxLayout(zoomGroup);
    auto zoomInButton = new QPushButton("Aumentar Zoom (+)");
    auto zoomOutButton = new QPushButton("Diminuir Zoom (-)");
    zoomLayout->addWidget(zoomInButton);
    zoomLayout->addWidget(zoomOutButton);

    controlLayout->addWidget(windowGroup);
    // controlLayout->addWidget(viewportGroup);
    controlLayout->addWidget(zoomGroup);
    controlLayout->addStretch();

    mainLayout->addWidget(controlPanel);
    setCentralWidget(centralWidget);

    connect(winX, &QDoubleSpinBox::valueChanged, this, &MainWindow::atualizarTransformacoes);
    connect(winY, &QDoubleSpinBox::valueChanged, this, &MainWindow::atualizarTransformacoes);
    connect(winW, &QDoubleSpinBox::valueChanged, this, &MainWindow::atualizarTransformacoes);
    connect(winH, &QDoubleSpinBox::valueChanged, this, &MainWindow::atualizarTransformacoes);

    connect(m_canvas, &Canvas::viewportAlterado, this, &MainWindow::onViewportAlterado);

    connect(zoomInButton, &QPushButton::clicked, this, &MainWindow::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &MainWindow::zoomOut);
}

void MainWindow::atualizarTransformacoes()
{
    m_window.setX(winX->value());
    m_window.setY(winY->value());
    m_window.setWidth(winW->value());
    m_window.setHeight(winH->value());

    m_canvas->definirMundo(m_window); // nova window para clipping

    recalcularTransformacao();
}

void MainWindow::onViewportAlterado(const QRectF &viewport)
{
    m_viewport = viewport;
    atualizarUiPeloEstado();
    recalcularTransformacao();
}


void MainWindow::recalcularTransformacao()
{
    if (m_window.width() == 0 || m_window.height() == 0) return;

    QTransform transform;
    transform.translate(m_viewport.x(), m_viewport.y());
    transform.scale(m_viewport.width() / m_window.width(), m_viewport.height() / m_window.height());
    transform.translate(-m_window.x(), -m_window.y());

    m_transformacao = transform;
    m_canvas->definirTransformacao(m_transformacao);
}

void MainWindow::atualizarUiPeloEstado()
{
    QSignalBlocker blockerX(winX);
    QSignalBlocker blockerY(winY);
    QSignalBlocker blockerW(winW);
    QSignalBlocker blockerH(winH);

    winX->setValue(m_window.x());
    winY->setValue(m_window.y());
    winW->setValue(m_window.width());
    winH->setValue(m_window.height());
}

void MainWindow::zoomIn()
{
    const double zoomFactor = 1.25;
    QPointF center = m_window.center();
    double newWidth = m_window.width() / zoomFactor;
    double newHeight = m_window.height() / zoomFactor;

    m_window.setWidth(newWidth);
    m_window.setHeight(newHeight);
    m_window.moveCenter(center);

    atualizarUiPeloEstado();
    atualizarTransformacoes();
}

void MainWindow::zoomOut()
{
    const double zoomFactor = 1.25;
    QPointF center = m_window.center();
    double newWidth = m_window.width() * zoomFactor;
    double newHeight = m_window.height() * zoomFactor;

    m_window.setWidth(newWidth);
    m_window.setHeight(newHeight);
    m_window.moveCenter(center);

    atualizarUiPeloEstado();
    atualizarTransformacoes();
}

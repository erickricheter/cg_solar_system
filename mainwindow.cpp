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

// MUDANÇA: Novos includes para o carregador .obj e depuração
#include <QFile>
#include <QTextStream>
#include <QDebug>

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

    m_window.setRect(-100, -100, 200, 200);

    // --- MUDANÇA: Adicionar Cor e Chão ---

    // 1. Criar o Chão (Gramado)
    auto ground = new Objeto3D("Chao");
    // Z um pouco abaixo de 0 para os Pokémon ficarem "em cima"
    float groundZ = -10.0; // Ajuste este valor se os Pokémon estiverem flutuando
    ground->m_vertices.append(QVector3D(-500, -500, groundZ)); // v0
    ground->m_vertices.append(QVector3D( 500, -500, groundZ)); // v1
    ground->m_vertices.append(QVector3D( 500,  500, groundZ)); // v2
    ground->m_vertices.append(QVector3D(-500,  500, groundZ)); // v3
    // 2 faces triangulares para formar um quadrado
    ground->m_faces.append({0, 1, 2}); // Triângulo 1
    ground->m_faces.append({0, 2, 3}); // Triângulo 2
    ground->setColor(QColor(34, 139, 34)); // Verde Grama
    m_displayFile.append(ground);

    // 2. Carregar Pokémon 1 com Cor
    auto pokemon1 = new Objeto3D("Charizard");
    if (carregarObjetoDeArquivo("charizard.obj", pokemon1)) {
        pokemon1->m_modelMatrix.scale(0.5);
        pokemon1->m_modelMatrix.translate(-50, 0, 0);
        pokemon1->setColor(QColor(255, 69, 0)); // Laranja
        m_displayFile.append(pokemon1);
    } else {
        qDebug() << "ERRO: Falha ao carregar charizard.obj";
    }

    // 3. Carregar Pokémon 2 com Cor
    auto pokemon2 = new Objeto3D("Blastoise");
    if (carregarObjetoDeArquivo("blastoise.obj", pokemon2)) {
        pokemon2->m_modelMatrix.scale(0.5);
        pokemon2->m_modelMatrix.translate(50, 0, 0);
        pokemon2->setColor(QColor(0, 0, 205)); // Azul
        m_displayFile.append(pokemon2);
    } else {
        qDebug() << "ERRO: Falha ao carregar blastoise.obj";
    }

    m_canvas = new Canvas(this);
    m_canvas->definirDisplayFile3D(&m_displayFile);

    setupUiControls();

    // --- MUDANÇA: Iniciar Animação ---
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &MainWindow::onAnimationTick);
    m_animationTimer->start(30); // Atualiza a cada ~30ms (~33 FPS)
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_displayFile);
    delete ui;
}
// MUDANÇA: Funções 2D removidas
// ObjetoGrafico MainWindow::criarPlaneta(...) { ... }
// ObjetoGrafico MainWindow::criarOrbita(...) { ... }


// MUDANÇA: Nova função para carregar .obj
bool MainWindow::carregarObjetoDeArquivo(const QString &caminho, Objeto3D *objeto)
{
    QFile file(caminho);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erro: Não foi possível abrir o arquivo" << caminho;
        return false;
    }
    objeto->m_vertices.clear();
    objeto->m_faces.clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        if (parts.isEmpty() || line.startsWith('#')) continue;

        if (parts[0] == "v" && parts.size() >= 4) { // Vértice
            objeto->m_vertices.append(QVector3D(parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat()));
        } else if (parts[0] == "f" && parts.size() >= 4) { // Face
            int v1 = parts[1].split('/')[0].toInt();
            int v2 = parts[2].split('/')[0].toInt();
            int v3 = parts[3].split('/')[0].toInt();
            // .obj é 1-based, QVector é 0-based. Subtraia 1!
            objeto->m_faces.append({v1 - 1, v2 - 1, v3 - 1});
        }
    }
    file.close();
    qDebug() << "Carregado" << caminho << "| Vértices:" << objeto->m_vertices.size() << "Faces:" << objeto->m_faces.size();
    return !objeto->m_vertices.isEmpty() && !objeto->m_faces.isEmpty();
}


void MainWindow::setupUiControls()
{
    // ... (Esta função continua a mesma, pois os controles da window são os mesmos)
    // ... (Apenas cole seu código de setupUiControls() aqui)
    // ... (Lembre-se de ajustar os ranges dos spin boxes para a nova window)
    // Ex: winX = createSpinBox(-2000, 2000, m_window.x());
    //     winW = createSpinBox(1, 4000, m_window.width());
    //     ...
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

    // MUDANÇA: Não precisamos mais do definirMundo no canvas 2D
    // m_canvas->definirMundo(m_window);

    recalcularTransformacao();
}

void MainWindow::onViewportAlterado(const QRectF &viewport)
{
    m_viewport = viewport;
    atualizarUiPeloEstado(); // Atualiza a UI se ela dependesse da viewport
    recalcularTransformacao();
}


void MainWindow::recalcularTransformacao()
{
    if (m_window.width() == 0 || m_window.height() == 0) return;

    QMatrix4x4 projectionMatrix;
    float nearPlane = -1000.0;
    float farPlane = 1000.0;
    projectionMatrix.ortho(
        m_window.left(),
        m_window.right(),
        m_window.bottom(),
        m_window.top(),
        nearPlane,
        farPlane
        );

    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(
        QVector3D(0, -200, 100), // Posição da Câmera (olhando de 'frente' e de 'cima')
        QVector3D(0, 0, 0),    // Ponto para onde a câmera olha
        QVector3D(0, 0, 1)     // MUDANÇA: Vetor "para cima" agora é Z!
        );

    m_viewProjectionMatrix = projectionMatrix * viewMatrix;

    m_canvas->definirTransformacao3D(m_viewProjectionMatrix, m_viewport);
}

// --- MUDANÇA: NOVO SLOT PARA ANIMAÇÃO ---
void MainWindow::onAnimationTick()
{
    // Gira os Pokémon (mas não o chão)
    for (Objeto3D* obj : m_displayFile) {
        if (obj->m_nome != "Chao") {
            // Gira 1 grau ao redor do eixo Z (o eixo "para cima")
            obj->m_modelMatrix.rotate(1.0, 0, 0, 1);
        }
    }

    // Manda o canvas redesenhar com as novas posições
    m_canvas->update();
}
void MainWindow::atualizarUiPeloEstado()
{
    // ... (Esta função continua a mesma) ...
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
    // ... (Esta função continua a mesma, ela manipula m_window) ...
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
    // ... (Esta função continua a mesma, ela manipula m_window) ...
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


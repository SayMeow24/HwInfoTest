#include "MainWindow.h"
#include <QStyleFactory>
#include <QClipboard>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    // Темна тема
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QPalette dark;
    dark.setColor(QPalette::Window, QColor(45, 45, 45));
    dark.setColor(QPalette::WindowText, Qt::white);
    dark.setColor(QPalette::Base, QColor(30, 30, 30));
    dark.setColor(QPalette::Text, Qt::white);
    QApplication::setPalette(dark);

    // UI
    tabs = new QTabWidget(this);

    structureText = new QTextEdit();
    quickText = new QTextEdit();
    jsonText = new QTextEdit();

    structureText->setReadOnly(true);
    quickText->setReadOnly(true);
    jsonText->setReadOnly(true);

    tabs->addTab(structureText, "Structure");
    tabs->addTab(quickText, "Quick Access");
    tabs->addTab(jsonText, "JSON");

    copyJsonBtn = new QPushButton("Copy JSON");

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tabs);

    auto* bottom = new QHBoxLayout();
    bottom->addStretch();
    bottom->addWidget(copyJsonBtn);
    layout->addLayout(bottom);

    connect(tabs, &QTabWidget::currentChanged, this, [this](int i) {
        if (i == 0) updateStructureTab();
        if (i == 1) updateQuickAccessTab();
        if (i == 2) updateJSONTab();
        });

    connect(copyJsonBtn, &QPushButton::clicked, this, &MainWindow::copyJSON);

    updateStructureTab();
}

//
// ========= STRUCTURE TAB (REAL DATA) =========
//
void MainWindow::updateStructureTab()
{
    ArgentumDevice d = provider.getDeviceInfo();
    structureText->setPlainText(buildStructure(d));
}

QString MainWindow::buildStructure(const ArgentumDevice& d)
{
    QString out;

    out += "=== ArgentumDevice Structure ===\n\n";

    out += "Platform: " + QString::fromStdString(d.platform.value_or("Unknown")) + "\n\n";

    out += "OS:\n";
    out += "  Name: " + QString::fromStdString(d.os) + "\n";
    out += "  Kernel: " + QString::fromStdString(d.os_kernel.value_or("Unknown")) + "\n";
    out += "  Architecture: " + QString::fromStdString(d.os_arch.value_or("Unknown")) + "\n\n";

    out += "CPU:\n";
    out += "  Model: " + QString::fromStdString(d.cpu_model.value_or("Unknown")) + "\n";
    out += "  Cores: " + QString::number(d.cpu_cores) + "\n";
    if (d.cpu_frequency_mhz)
        out += "  Frequency: " + QString::number(d.cpu_frequency_mhz.value()) + " MHz\n";
    out += "\n";

    out += "RAM:\n";
    out += "  Total: " + QString::number(d.ram_mb) + " MB\n";
    if (d.ram_available_mb)
        out += "  Available: " + QString::number(d.ram_available_mb.value()) + " MB\n";
    if (d.ram_used_mb)
        out += "  Used: " + QString::number(d.ram_used_mb.value()) + " MB\n";
    if (d.ram_usage_percent)
        out += "  Usage: " + QString::number(d.ram_usage_percent.value()) + "%\n";
    out += "\n";

    out += "GPU (" + QString::number(d.gpus.size()) + " detected)\n";
    for (size_t i = 0; i < d.gpus.size(); i++)
    {
        const auto& g = d.gpus[i];
        out += "\n  GPU " + QString::number(i + 1) + ": " + QString::fromStdString(g.model) + "\n";
        if (g.vram_mb)
            out += "    VRAM: " + QString::number(g.vram_mb.value()) + " MB\n";
        if (g.vram_used_mb)
            out += "    Used: " + QString::number(g.vram_used_mb.value()) + " MB\n";
        if (g.vram_free_mb)
            out += "    Free: " + QString::number(g.vram_free_mb.value()) + " MB\n";
    }
    out += "\n";

    out += "Disks:\n";
    for (const auto& dsk : d.disks)
    {
        out += "\n  " + QString::fromStdString(dsk.mount_point) +
            " (" + QString::fromStdString(dsk.filesystem) + ")\n";

        out += "    Type: " + QString::fromStdString(HardwareInfoProvider::diskTypeToStdString(dsk.type)) + "\n";
        out += "    Size: " + QString::number(dsk.total_mb) + " MB\n";
        out += "    Free: " + QString::number(dsk.free_mb) + " MB\n";
        out += "    Used: " + QString::number(dsk.used_mb) + " MB\n";
    }

    return out;
}

//
// ========= QUICK ACCESS =========
//
void MainWindow::updateQuickAccessTab()
{
    ArgentumDevice d = provider.getDeviceInfo();
    quickText->setPlainText(buildQuick(d));
}

QString MainWindow::buildQuick(const ArgentumDevice& d)
{
    QString out;

    out += "--- Quick Access ---\n\n";

    out += "OS: " + QString::fromStdString(d.os) + "\n";
    out += "CPU: " + QString::fromStdString(d.cpu_model.value_or("")) + "\n";
    out += "Cores: " + QString::number(d.cpu_cores) + "\n";

    if (d.ram_usage_percent)
        out += "RAM Usage: " + QString::number(d.ram_usage_percent.value()) + "%\n";

    if (!d.gpus.empty())
        out += "Primary GPU: " + QString::fromStdString(d.gpus[0].model) + "\n";

    out += "Disks: " + QString::number(d.disks.size()) + "\n";

    return out;
}

//
// ========= JSON TAB =========
//
void MainWindow::updateJSONTab()
{
    ArgentumDevice d = provider.getDeviceInfo();
    jsonText->setPlainText(buildJSON(d));
}

QString MainWindow::buildJSON(const ArgentumDevice& d)
{
    QJsonObject root;

    root["os"] = QString::fromStdString(d.os);
    root["cpu_cores"] = int(d.cpu_cores);
    root["ram_mb"] = qint64(d.ram_mb);

    QJsonArray gpuArr;
    for (auto& g : d.gpus)
    {
        QJsonObject gJ;
        gJ["model"] = QString::fromStdString(g.model);
        if (g.vram_mb) gJ["vram_mb"] = int(g.vram_mb.value());
        gpuArr.append(gJ);
    }
    root["gpus"] = gpuArr;

    QJsonArray diskArr;
    for (auto& dsk : d.disks)
    {
        QJsonObject o;
        o["mount"] = QString::fromStdString(dsk.mount_point);
        o["total_mb"] = int(dsk.total_mb);
        o["free_mb"] = int(dsk.free_mb);
        diskArr.append(o);
    }
    root["disks"] = diskArr;

    return QJsonDocument(root).toJson(QJsonDocument::Indented);
}

//
// ========= COPY JSON BUTTON =========
//
void MainWindow::copyJSON()
{
    QApplication::clipboard()->setText(jsonText->toPlainText());
}

#include "MainWindow.moc"

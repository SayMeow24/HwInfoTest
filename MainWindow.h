#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QClipboard>
#include <QApplication>
#include "HardwareInfoProvider.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void updateStructureTab();
    void updateQuickAccessTab();
    void updateJSONTab();
    void copyJSON();

private:
    QTabWidget* tabs;

    QTextEdit* structureText;
    QTextEdit* quickText;
    QTextEdit* jsonText;

    QPushButton* copyJsonBtn;

    HardwareInfoProvider provider;

    // ❗❗❗ Саме ці 3 функції були відсутні => LNK2019 ❗❗❗
    QString buildStructure(const ArgentumDevice&);
    QString buildQuick(const ArgentumDevice&);
    QString buildJSON(const ArgentumDevice&);
};

#endif // MAINWINDOW_H

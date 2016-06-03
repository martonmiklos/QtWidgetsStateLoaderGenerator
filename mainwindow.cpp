#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QCheckBox>

#include "mainwindow_settings.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_actionNew_project_triggered();  // add a blank project tab

    m_statusLabel = new QLabel(this);
    ui->statusBar->addWidget(m_statusLabel);
    loadWidgetStates();
}

MainWindow::~MainWindow()
{
    saveWidgetStates();
    delete ui;
}

void MainWindow::info(QString info)
{
    m_statusLabel->setText(info);
}

void MainWindow::on_actionLoad_UI_file_triggered()
{
    QSettings settings;
    QString uiPath = QFileDialog::getOpenFileName(
                this,
                tr("Select the .ui file"),
                settings.value("lastUIDir").toString(),
                tr("UI files (*.ui)")
    );

    if (uiPath != "") {
        QFileInfo fi(uiPath);
        settings.setValue("lastUIDir", fi.dir().path());
        m_tabWidgets.at(m_currentTabIndex)->importUIFile(uiPath);
        ui->tabWidget->setTabText(m_currentTabIndex, fi.fileName());
        this->repaint();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QSettings settings;
    settings.beginGroup("paths");
    QString projectPath = QFileDialog::getOpenFileName(
                this,
                tr("Select the project file"),
                settings.value("lastProjectDir", QDir::homePath()).toString(),
                tr("INI files (*.ini)")
    );

    if (projectPath != "") {
        WidgetSettingsSaver *ws = m_tabWidgets.at(ui->tabWidget->currentIndex());
        if (ws->loadProject(projectPath)) {
            QFileInfo fi(projectPath);
            settings.setValue("lastProjectDir", fi.absolutePath());
        } else {
            m_statusLabel->setText(ws->error());
        }
    }
    settings.endGroup();
}

void MainWindow::on_actionNew_project_triggered()
{
    WidgetSettingsSaver *newTabWidget = new WidgetSettingsSaver(ui->tabWidget);
    connect(newTabWidget, SIGNAL(info(QString)), this, SLOT(info(QString)));
    ui->tabWidget->addTab(newTabWidget, tr("No name"));
    m_tabWidgets.append(newTabWidget);
    ui->actionSave->setEnabled(true);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    m_currentTabIndex = index;
    ui->actionLoad_UI_file->setEnabled(true);
}

void MainWindow::on_actionSave_triggered()
{
    WidgetSettingsSaver *ws = m_tabWidgets.at(ui->tabWidget->currentIndex());
    if (ws->saveProject()) {
        m_statusLabel->setText(tr("Project saved"));
    } else {
        m_statusLabel->setText(tr("Project save failed: %1").arg(ws->error()));
    }
}

void MainWindow::on_actionQuit_triggered()
{
    bool exit = false;
    QSettings settings;
    settings.beginGroup("MainWindow");
    if (settings.value("quitWithoutPrompt").toBool()) {
        exit = true;
    } else {
        QMessageBox quitQuestion;
        QCheckBox checkBox;
        checkBox.setText(tr("Do not ask again"));
        quitQuestion.setCheckBox(&checkBox);
        quitQuestion.setText(tr("Do you really want to exit this program?"));
        quitQuestion.setWindowTitle(tr("Are you sure?"));
        quitQuestion.setStandardButtons(QMessageBox::No	| QMessageBox::Yes);
        quitQuestion.setDefaultButton(QMessageBox::No);
        quitQuestion.exec();
        if (quitQuestion.result() == QMessageBox::Yes) {
            settings.setValue("quitWithoutPrompt", checkBox.isChecked());
            exit = true;
        }
    }
    settings.endGroup();
    if (exit)
        QCoreApplication::quit();
}

void MainWindow::setQuitQuestinEnabled()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("quitWithoutPrompt", false);
    settings.endGroup();
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{

}

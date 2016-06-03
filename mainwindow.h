#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDomDocument>
#include <QSettings>
#include <QLabel>

#include "widgetsettingssaver.h"
#include "settingssavermodel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setQuitQuestinEnabled();
public slots:
    void info(QString info);

private slots:
    void on_actionLoad_UI_file_triggered();
    void on_actionOpen_triggered();
    void on_actionNew_project_triggered();
    void on_tabWidget_currentChanged(int index);
    void on_actionSave_triggered();
    void on_actionQuit_triggered();
    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::MainWindow *ui;
    void loadWidgetStates();
    void saveWidgetStates();

    SettingsSaverModel *m_model;
    QList<WidgetSettingsSaver*> m_tabWidgets;
    int m_currentTabIndex;
    QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H

#ifndef WIDGETSETTINGSSAVER_H
#define WIDGETSETTINGSSAVER_H

#include <QWidget>

#include "settingssavermodel.h"

namespace Ui {
class WidgetSettingsSaver;
}

class WidgetSettingsSaver : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSettingsSaver(QWidget *parent = 0);
    bool importUIFile(QString path);
    ~WidgetSettingsSaver();

    QString error() const;
    void setError(const QString &error);

    bool saveProject();
    bool loadProject(QString projectPath);

private slots:
    void on_pushButtonGenerate_clicked();
    void on_toolButtonSelectOutputDir_clicked();
    void on_pushButtonPreview_clicked();

signals:
    void info(QString info);

private:
    Ui::WidgetSettingsSaver *ui;
    SettingsSaverModel *m_model;
    QString m_uiFilePath, m_projectPath;
    QString m_error;
};

#endif // WIDGETSETTINGSSAVER_H

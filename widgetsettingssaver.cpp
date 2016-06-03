#include "widgetsettingssaver.h"
#include "ui_widgetsettingssaver.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QSettings>

WidgetSettingsSaver::WidgetSettingsSaver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSettingsSaver)
{
    ui->setupUi(this);

    m_model = new SettingsSaverModel(this);
    ui->tableView->setModel(m_model);
}

WidgetSettingsSaver::~WidgetSettingsSaver()
{
    delete ui;
}

bool WidgetSettingsSaver::importUIFile(QString path)
{
    bool importResult = m_model->importUIFile(path);
    m_uiFilePath = path;
    ui->tableView->reset();
    if (importResult) {
        ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

        QFileInfo fi(path);
        ui->lineEditOutputFile->setText(fi.path() + QDir::separator() + fi.baseName()+ "_settings.cpp");
        ui->lineEditUI_File->setText("ui_" + fi.baseName() + ".h");
        ui->uIImplementationHeaderFileLineEdit->setText(fi.baseName() + ".h");
        emit info(tr("The %1 ui file imported").arg(fi.baseName()));
    } else {
        m_error = m_model->error();
    }
    return importResult;
}

void WidgetSettingsSaver::on_pushButtonGenerate_clicked()
{

    if (m_model->saveToFile(ui->lineEditOutputFile->text(),
                            ui->uIImplementationHeaderFileLineEdit->text(),
                            ui->lineEditUI_File->text(),
                            ui->lineEditSaveMethod->text(),
                            ui->lineEditLoadMethod->text(),
                            ui->checkBoxPassArgument->isChecked(),
                            ui->lineEditArgument->text())) {
        emit info(tr("The file generated successfully!"));
    } else {
        QMessageBox::warning(this,
                             tr("Save failed"),
                             tr("Error happened during the file generation:\n%1").arg(m_model->error()));
    }

}


void WidgetSettingsSaver::on_toolButtonSelectOutputDir_clicked()
{
    QFileInfo fi(m_uiFilePath);
    QString outputFile = QFileDialog::getSaveFileName(
                this,
                tr("Select output file"),
                fi.absolutePath());
    if (outputFile != "") {
        ui->lineEditOutputFile->setText(outputFile);
    }
}

void WidgetSettingsSaver::on_pushButtonPreview_clicked()
{
    QTextEdit preview;
    //preview.setText();
}
QString WidgetSettingsSaver::error() const
{
    return m_error;
}

void WidgetSettingsSaver::setError(const QString &error)
{
    m_error = error;
}

bool WidgetSettingsSaver::saveProject()
{
    QFileInfo fi(m_uiFilePath);
    QString projectPath = m_projectPath;
    if (projectPath.isEmpty()) {
        projectPath = QFileDialog::getSaveFileName(this,
                                                   tr("Please select the project file to save"),
                                                   fi.absolutePath() + QDir::separator() + fi.baseName() + ".ini",
                                                   QString("*.ini")
                                                   );
    }

    if (!projectPath.isEmpty()) {
        QSettings settings(projectPath, QSettings::IniFormat);
        settings.beginGroup("project");
        settings.setValue("checkBoxPassArgument", ui->checkBoxPassArgument->isChecked());
        settings.setValue("lineEditOutputFile", ui->lineEditOutputFile->text());
        settings.setValue("lineEditUI_File", ui->lineEditUI_File->text());
        settings.setValue("uIImplementationHeaderFileLineEdit", ui->uIImplementationHeaderFileLineEdit->text());
        settings.setValue("lineEditSaveMethod", ui->lineEditSaveMethod->text());
        settings.setValue("lineEditLoadMethod", ui->lineEditLoadMethod->text());
        settings.setValue("lineEditArgument", ui->lineEditArgument->text());
        settings.setValue("uiFile", m_uiFilePath);
        settings.endGroup();

        settings.beginGroup("widgets");
        for (int i = 0; i<m_model->rowCount(QModelIndex()); i++) {
            settings.beginGroup(m_model->data(m_model->index(i, 0), Qt::DisplayRole).toString());
            settings.setValue("key", m_model->data(m_model->index(i, 2), Qt::DisplayRole));
            settings.setValue("section", m_model->data(m_model->index(i, 3), Qt::DisplayRole));
            settings.setValue("load", m_model->data(m_model->index(i, 4), Qt::CheckStateRole) == Qt::Checked);
            settings.setValue("save", m_model->data(m_model->index(i, 5), Qt::CheckStateRole) == Qt::Checked);
            settings.endGroup();
        }
        settings.endGroup();

        settings.sync();
        if (settings.status() == QSettings::NoError) {
            m_projectPath = projectPath;
            return true;
        } else {
            if (settings.status() == QSettings::AccessError)
                m_error = tr("Unable to write the %1 file").arg(projectPath);
            return false;
        }
    } else {
        m_error = tr("Project save cancelled by the user");
        return false;
    }
    m_error = tr("How the hell we got here?");
    return false;
}

bool WidgetSettingsSaver::loadProject(QString projectPath)
{
    QSettings settings(projectPath, QSettings::IniFormat);
    if (settings.status() == QSettings::NoError) {
        settings.beginGroup("project");
        ui->checkBoxPassArgument->setChecked(settings.value("checkBoxPassArgument").toBool());
        ui->lineEditOutputFile->setText(settings.value("lineEditOutputFile").toString());
        ui->lineEditUI_File->setText(settings.value("lineEditUI_File").toString());
        ui->uIImplementationHeaderFileLineEdit->setText(settings.value("uIImplementationHeaderFileLineEdit").toString());
        ui->lineEditSaveMethod->setText(settings.value("lineEditSaveMethod").toString());
        ui->lineEditLoadMethod->setText(settings.value("lineEditLoadMethod").toString());
        ui->lineEditArgument->setText(settings.value("lineEditArgument").toString());
        m_uiFilePath = settings.value("uiFile").toString();
        settings.endGroup();

        if (!importUIFile(m_uiFilePath)) {
            return false;
        }

        settings.beginGroup("widgets");
        for (int i = 0; i<m_model->rowCount(QModelIndex()); i++) {
            settings.beginGroup(m_model->data(m_model->index(i, 0), Qt::DisplayRole).toString());
            m_model->setData(m_model->index(i, 2), settings.value("key"), Qt::DisplayRole);
            m_model->setData(m_model->index(i, 3), settings.value("section"), Qt::DisplayRole);
            m_model->setData(
                        m_model->index(i, 4),
                        settings.value("load", m_model->data(m_model->index(i, 4), Qt::CheckStateRole) == Qt::Checked),
                        Qt::CheckStateRole);
            m_model->setData(
                        m_model->index(i, 5),
                        settings.value("save", m_model->data(m_model->index(i, 5), Qt::CheckStateRole) == Qt::Checked),
                        Qt::CheckStateRole);
            settings.endGroup();
        }
        settings.endGroup();
        m_projectPath = projectPath;
        return true;
    } else {
        m_error = tr("Unable to open the %1 project file for read").arg(projectPath);
        return false;
    }
    m_error = tr("How the hell we got here?");
    return false;
}


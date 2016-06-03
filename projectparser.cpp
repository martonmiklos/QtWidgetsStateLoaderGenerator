#include "projectparser.h"

#include <QSettings>

ProjectParser::ProjectParser(QObject *parent) :
    QObject(parent)
{
    m_model = new SettingsSaverModel(this);
}

bool ProjectParser::parseProject(QString projectPath)
{
    QSettings settings(projectPath, QSettings::IniFormat);
    if (settings.status() == QSettings::NoError) {
        settings.beginGroup("project");
        m_passArgument = settings.value("checkBoxPassArgument").toBool();
        m_outputFile = settings.value("lineEditOutputFile").toString();
        m_uiHeaderPath = settings.value("lineEditUI_File").toString();
        m_uiImplementationHeader = settings.value("uIImplementationHeaderFileLineEdit").toString();
        m_saveMethod = settings.value("lineEditSaveMethod").toString();
        m_loadMethod = settings.value("lineEditLoadMethod").toString();
        m_argument = settings.value("lineEditArgument").toString();
        m_uiFilePath = settings.value("uiFile").toString();
        settings.endGroup();

        if (!m_model->importUIFile(m_uiFilePath)) {
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
    } else {
        m_error = tr("Unable to open the %1 project file for read").arg(projectPath);
        return false;
    }
    m_error = tr("How the hell we got here?");
    return false;
}

bool ProjectParser::generate()
{
    return m_model->saveToFile(m_outputFile,
                        m_uiImplementationHeader,
                        m_uiHeaderPath,
                        m_saveMethod,
                        m_loadMethod,
                        m_passArgument,
                        m_argument);
}

QString ProjectParser::error() const
{
    return m_error;
}

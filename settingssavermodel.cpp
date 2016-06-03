#include "settingssavermodel.h"

#include <QFile>
#include <QDebug>

SettingsSaverModel::SettingsSaverModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_uiImporter = new UIFileImporter();

    beginInsertColumns(index(0, 0), 0, 4);
    for (int i = 0; i<5; i++) {
        insertColumn(i);
    }
    endInsertColumns();
}

QVariant SettingsSaverModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Widget name");
        case 1:
            return tr("Class");
        case 2:
            return tr("Key");
        case 3:
            return tr("Section");
        case 4:
            return tr("Load");
        case 5:
            return tr("Save");
        }
    }
    return QVariant();
}

QVariant SettingsSaverModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return m_elements.at(index.row())->widgetName();
        case 1:
            return m_elements.at(index.row())->className();
        case 2:
            return m_elements.at(index.row())->keyName();
        case 3:
            return m_elements.at(index.row())->section();
        }
    } else if (role == Qt::CheckStateRole) {
        if (index.column() == 4) {
            if (m_elements.at(index.row())->loadState())
                return Qt::Checked;
            else
                return Qt::Unchecked;

        } else if (index.column() == 5) {
            if (m_elements.at(index.row())->saveState())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
    } else if (role == Qt::EditRole) {
        switch (index.column()) {
        case 2:
            return m_elements.at(index.row())->keyName();
        case 3:
            return m_elements.at(index.row())->section();
        }
    }
    return QVariant();
}

bool SettingsSaverModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    switch (index.column()) {
    case 2:
        m_elements[index.row()]->setKeyName(value.toString());
        break;
    case 3:
        m_elements[index.row()]->setSection(value.toString());
        break;
    case 4:
        m_elements[index.row()]->setLoadState(value.toBool());
        break;
    case 5:
        m_elements[index.row()]->setSaveState(value.toBool());
        break;
    }
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags SettingsSaverModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = Qt::ItemIsEnabled;
    switch (index.column()) {
    case 2:
    case 3:
        ret |= Qt::ItemIsEditable;
        break;
    case 4:
    case 5:
        ret = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        break;
    }
    return ret;
}

int SettingsSaverModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_elements.size();
}

int SettingsSaverModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 6;
}

bool SettingsSaverModel::saveToFile(QString filePath,
                                    QString uiImplementationHeader,
                                    QString uiDeclarationHeader,
                                    QString saveMethod,
                                    QString loadMethod,
                                    bool passSettingAsArgument,
                                    QString argument)
{
    QString settingsObject = "settings";
    QString settingsObjectOperand = ".";
    if (passSettingAsArgument) {
        // argument name will be the last
        QStringList parts = argument.split(" ");
        if (parts.size() < 2) {
            m_error = tr("The passed argument is invalid: %1\n(Must have least two parts separated with space)").arg(argument);
            return false;
        } else {
            settingsObject = parts.last();
            if (settingsObject.startsWith('*') || settingsObject.startsWith('&')) {
                settingsObject = settingsObject.mid(1);
            }
        }

        if (argument.contains('*')) {
            settingsObjectOperand = "->";
        }
    } else {
        argument = "";
    }

    QFile outFile(filePath);
    if (outFile.open(QFile::ReadWrite | QFile::Truncate)) {
        // create a list from the sections
        QStringList sectionList;
        foreach (WidgetSettings *widgetSettings, m_elements) {
            if (!sectionList.contains(widgetSettings->section())) {
                sectionList.append(widgetSettings->section());
            }
        }

        // sort the sections to be able to print methods in section blocks
        sectionList.sort();

        outFile.write(QString("#include <QSettings>\n\n").toLocal8Bit());
        outFile.write(QString("#include \"%1\"\n").arg(uiImplementationHeader).toLocal8Bit());
        outFile.write(QString("#include \"%1\"\n\n").arg(uiDeclarationHeader).toLocal8Bit());

        // generate save method
        outFile.write(QString("void %1::%2(%3)\n{\n")
                      .arg(m_elements.first()->widgetName())
                      .arg(saveMethod)
                      .arg(argument)
                      .toLocal8Bit());

        if (!passSettingAsArgument)
            outFile.write(QString("\tQSettings settings;\n").toLocal8Bit());

        foreach (QString section, sectionList) {
            QString sectionText = "";
            foreach (WidgetSettings *settings, m_elements) {
                if (settings->section() == section) {
                    QString widgetSaveMethod = settings->saveMethod(settingsObject + settingsObjectOperand);
                    if (!widgetSaveMethod.isEmpty())
                        sectionText.append(QString("\t%1\n").arg(widgetSaveMethod));
                }
            }

            if (!sectionText.isEmpty()) {
                // write begingroup if we are in a section
                if (!section.isEmpty()) {
                    sectionText.prepend(QString("\n\t%1%2beginGroup(\"%3\");\n")
                                  .arg(settingsObject)
                                  .arg(settingsObjectOperand)
                                  .arg(section));
                }

                // write endgroup if we are in a section
                if (!section.isEmpty()) {
                    sectionText.append(QString("\t%1%2endGroup();\n\n")
                                  .arg(settingsObject)
                                  .arg(settingsObjectOperand));
                }

                outFile.write(sectionText.toLocal8Bit());
            }
        }
        outFile.write("}\n\n");

        outFile.write(QString("void %1::%2(%3)\n{\n")
                      .arg(m_elements.first()->widgetName())
                      .arg(loadMethod)
                      .arg(argument)
                      .toLocal8Bit());

        if (!passSettingAsArgument)
            outFile.write(QString("\tQSettings settings;\n").toLocal8Bit());
        foreach (QString section, sectionList) {
            QString sectionText = "";
            foreach (WidgetSettings *settings, m_elements) {
                if (settings->section() == section) {
                    QString widgetLoadMethod = settings->loadMethod(settingsObject + settingsObjectOperand);
                    if (!widgetLoadMethod.isEmpty())
                        sectionText.append(QString("\t%1\n").arg(widgetLoadMethod));
                }
            }

            if (!sectionText.isEmpty()) {
                // TODO be DRY!
                // write begingroup if we are in a section
                if (!section.isEmpty()) {
                    sectionText.prepend(QString("\n\t%1%2beginGroup(\"%3\");\n")
                                        .arg(settingsObject)
                                        .arg(settingsObjectOperand)
                                        .arg(section));
                }

                // write endgroup if we are in a section
                if (!section.isEmpty()) {
                    sectionText.append(QString("\t%1%2endGroup();\n")
                                       .arg(settingsObject)
                                       .arg(settingsObjectOperand));
                }
                outFile.write(sectionText.toLocal8Bit());
            }
        }
        outFile.write("}\n\n");

        outFile.close();
        m_error = "";
        return true;
    } else {
        m_error = tr("Unable to open the %1 file for write").arg(filePath);
        return false;
    }
    return false;
}

QString SettingsSaverModel::generate(QString filePath, QString uiImplementationHeader, QString uiDeclarationHeader, bool passSettingAsArgument, QString argument)
{

}

bool SettingsSaverModel::importUIFile(QString filePath)
{
    beginResetModel();

    bool ok = true;
    m_elements = m_uiImporter->importUIFile(filePath, &ok);

    endResetModel();
    return ok;
}
QString SettingsSaverModel::error() const
{
    return m_error;
}

WidgetSettings::WidgetSettings(QDomElement element)
{
    m_className = element.attribute("class");
    m_widgetName = element.attribute("name");
    m_keyName = m_widgetName;
    m_loadState = true;
    m_saveState = true;
}

/**
 * @brief WidgetSettings::saveMethod
 * @param settingsObjectName The QSetting instance name including the operand (. or ->)
 * @return Returns a string containing the widget state save to the QSettings object
 */
QString WidgetSettings::saveMethod(QString settingsObjectName)
{
    if (m_saveState == false)
        return "";

    QString variantValue = getValueString();
    if (variantValue.isEmpty())
        return ""; // return with empty string for non savable widgets
    return QString("%1setValue(\"%2\", %3);").arg(settingsObjectName).arg(m_keyName).arg(variantValue);
}

/**
 * @brief WidgetSettings::loadMethod
 * @param settingsObjectName The QSetting instance name including the operand (. or ->)
 * @return Returns a string containing the method which will apply the state readed from the settings to the widget
 */
QString WidgetSettings::loadMethod(QString settingsObjectName)
{
    if (m_className == "QCheckBox" ||
            m_className == "QRadioButton" ||
            m_className == "QPushButton")
        return QString("ui->%1->setChecked(%2value(\"%3\").toBool());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if (m_className == "QLineEdit")
        return QString("ui->%1->setText(%2value(\"%3\").toString());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if (m_className == "QPlainTextEdit")
        return QString("ui->%1->setPlainText(%2value(\"%3\").toString());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if (m_className == "QSpinBox" ||
             m_className == "QSlider" ||
             m_className == "QDial")
        return QString("ui->%1->setValue(%2value(\"%3\").toInt());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if(m_className == "QDoubleSpinBox")
        return QString("ui->%1->setValue(%2value(\"%3\").toDouble());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if(m_className == "QTextEdit")
        return QString("ui->%1->setHtml(%2value(\"%3\").toString());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if(m_className == "QDateEdit")
        return QString("ui->%1->setDate(%2value(\"%3\").toDate());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if(m_className == "QDateTimeEdit")
        return QString("ui->%1->setDateTime(%2.alue(\"%3\").toDateTime());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if(m_className == "QTimeEdit")
        return QString("ui->%1->setTime(%2value(\"%3\").toTime());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if (m_className == "QComboBox" ||
             m_className == "QTabWidget" ||
             m_className == "QTabView")
        return QString("ui->%1->setCurrentIndex(%2value(\"%3\").toInt());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    else if (m_className == "QWidget")
        return QString("ui->%1->restoreGeometry(%2value(\"%3\").toByteArray());")
                .arg(m_widgetName)
                .arg(settingsObjectName)
                .arg(m_keyName);
    return "";
}

QString WidgetSettings::getValueString()
{
    if (m_className == "QCheckBox" ||
            m_className == "QRadioButton" ||
            m_className == "QPushButton")
        return QString("ui->%1->isChecked()").arg(m_widgetName);
    else if (m_className == "QLineEdit")
        return QString("ui->%1->text()").arg(m_widgetName);
    else if(m_className == "QPlainTextEdit")
        return QString("ui->%1->toPlainText()").arg(m_widgetName);
    else if (m_className == "QTextEdit")
        return QString("ui->%1->toHtml()").arg(m_widgetName);
    else if (m_className == "QSpinBox" ||
             m_className == "QDoubleSpinBox" ||
             m_className == "QSlider" ||
             m_className == "QDial")
        return QString("ui->%1->value()").arg(m_widgetName);
    else if (m_className == "QDateEdit")
        return QString("ui->%1->date()").arg(m_widgetName);
    else if (m_className == "QDateTimeEdit")
        return QString("ui->%1->dateTime()").arg(m_widgetName);
    else if (m_className == "QTimeEdit")
        return QString("ui->%1->time()").arg(m_widgetName);
    else if (m_className == "QComboBox" ||
             m_className == "QTabWidget" ||
             m_className == "QTabView")
        return QString("ui->%1->currentIndex()").arg(m_widgetName);
    else if (m_className == "QWidget")
        return QString("ui->%1->saveGeometry()").arg(m_widgetName);
    return "";
}


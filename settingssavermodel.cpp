#include "settingssavermodel.h"

#include <QFile>
#include <QDebug>

#include "code.h"

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
    QString settingsObjectName = "settings";
    QString settingsObjectOperand = ".";
    if (passSettingAsArgument) {
        // argument name will be the last
        QStringList parts = argument.split(" ");
        if (parts.size() < 2) {
            m_error = tr("The passed argument is invalid: %1(Must have least two parts separated with space)").arg(argument);
            return false;
        } else {
            settingsObjectName = parts.last();
            if (settingsObjectName.startsWith('*') || settingsObjectName.startsWith('&')) {
                settingsObjectName = settingsObjectName.mid(1);
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

        bool writeLineEditCompleters = false;
        foreach (WidgetSettings *settings, m_elements) {
            if (settings->getCompleterForLineEdit()) {
                writeLineEditCompleters = true;
                break;
            }
        }

        KODE::Code code;
        code.setDefaultIndentation(4);
        code.addLine("#include <QSettings>");
        if (writeLineEditCompleters)
            code.addLine("#include <QCompleter>");
        code.newLine();
        code.addLine(QString("#include \"%1\"").arg(uiImplementationHeader));
        code.addLine(QString("#include \"%1\"").arg(uiDeclarationHeader));
        code.newLine();
        code.newLine();


        if (writeLineEditCompleters) {
            code.addLine("void "+m_elements.first()->widgetName()+"::saveLineEditCompleter(QLineEdit *lineEdit, QSettings & settings, int maxCount)");
            code.addLine("{");
            code.addLine("    if (lineEdit->text().isEmpty())");
            code.addLine("        return;");
            code.newLine();
            code.addLine("    QStringList completerList;");
            code.addLine("    bool found = false;");
            code.addLine("    settings.beginGroup(lineEdit->objectName() + \"_history\");");
            code.addLine("    int count = settings.beginReadArray(\"completer\");");
            code.addLine("    for (int i = 0; i<count; i++) {");
            code.addLine("        settings.setArrayIndex(i);");
            code.addLine("        QString value = settings.value(\"value\").toString();");
            code.addLine("        if (value != lineEdit->text()) {");
            code.addLine("            completerList << settings.value(\"value\").toString();");
            code.addLine("        } else {");
            code.addLine("            found = true;");
            code.addLine("        }");
            code.addLine("    }");
            code.addLine("    settings.endArray();");
            code.addLine("    settings.endGroup();");
            code.newLine();
            code.addLine("    if (!found) {");
            code.addLine("        completerList << lineEdit->text();");
            code.addLine("        if (maxCount > 0)");
            code.addLine("            completerList = completerList.mid(completerList.count() - maxCount);");
            code.newLine();
            code.addLine("        settings.beginGroup(lineEdit->objectName() + \"_history\");");
            code.addLine("        settings.beginWriteArray(\"completer\");");
            code.addLine("        for (int i = 0; i<completerList.size(); i++) {");
            code.addLine("            settings.setArrayIndex(i);");
            code.addLine("            settings.setValue(\"value\", completerList[i]);");
            code.addLine("        }");
            code.addLine("        settings.endArray();");
            code.addLine("        settings.endGroup();");
            code.addLine("    }");
            code.addLine("}");
            code.newLine();
            code.addLine("void "+m_elements.first()->widgetName()+"::loadLineEditCompleter(QLineEdit *lineEdit, QSettings &settings, int maxCount)");
            code.addLine("{");
            code.addLine("    QStringList completerList;");
            code.addLine("    settings.beginGroup(lineEdit->objectName() + \"_history\");");
            code.addLine("    int count = settings.beginReadArray(\"completer\");");
            code.addLine("    for (int i = 0; i<count; i++) {");
            code.addLine("        settings.setArrayIndex(i);");
            code.addLine("        completerList << settings.value(\"value\").toString();");
            code.addLine("    }");
            code.addLine("    settings.endArray();");
            code.addLine("    settings.endGroup();");
            code.newLine();
            code.addLine("    if (maxCount > 0)");
            code.addLine("        completerList = completerList.mid(completerList.count() - maxCount);");
            code.newLine();
            code.addLine("    QCompleter *lineEditCompeter = new QCompleter(completerList, lineEdit);");
            code.addLine("    lineEdit->setCompleter(lineEditCompeter);");
            code.addLine("}");
            code.newLine();
            code.newLine();
        }

        // generate save method
        code.addLine(QString("void %1::%2(%3) const")
                      .arg(m_elements.first()->widgetName())
                      .arg(saveMethod)
                      .arg(argument));
        code.addLine("{");
        code.indent();
        if (!passSettingAsArgument)
            code.addLine(QString("QSettings settings;"));

        foreach (QString sectionName, sectionList) {
            foreach (WidgetSettings *settings, m_elements) {
                if (settings->section() == sectionName) {
                    settings->generateSaveMethod(&code, settingsObjectName, settingsObjectOperand, sectionName);
                }
            }
        }
        code.unindent();
        code.addLine("}");
        code.newLine();

        // generate load method
        code.addLine(QString("void %1::%2(%3)")
                      .arg(m_elements.first()->widgetName())
                      .arg(loadMethod)
                      .arg(argument)
                      );
        code.addLine("{");
        code.indent();
        if (!passSettingAsArgument)
            code.addLine(QString("QSettings settings;"));
        foreach (QString sectionName, sectionList) {
            foreach (WidgetSettings *settings, m_elements) {
                if (settings->section() == sectionName) {
                    settings->generateLoadMethod(&code, settingsObjectName, settingsObjectOperand, sectionName);
                }
            }
        }
        code.unindent();
        code.addLine("}");
        outFile.write(code.text().toLocal8Bit());
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
    return "to be implemented";
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

WidgetSettings::WidgetSettings(QDomElement widgetElement, QDomNode customWidgetsNode) :
    m_topLevel(false)
{
    m_className = widgetElement.attribute("class");

    bool customFound = false;
    for (int i = 0; i<customWidgetsNode.childNodes().count(); i++) {
        bool isCustom = false;
        for (int j = 0; j<customWidgetsNode.childNodes().at(i).childNodes().count(); j++) {
            if (customWidgetsNode.childNodes().at(i).childNodes().at(j).toElement().tagName() == "class"
                    && customWidgetsNode.childNodes().at(i).childNodes().at(j).toElement().text() == m_className) {
                isCustom = true;
                customFound = true;
            }

            if (isCustom && customWidgetsNode.childNodes().at(i).childNodes().at(j).toElement().tagName() == "extends") {
                m_className = customWidgetsNode.childNodes().at(i).childNodes().at(j).toElement().text();
                break;
            }
        }
        if (customFound)
            break;
    }

    m_widgetName = widgetElement.attribute("name");
    m_keyName = m_widgetName;
    m_loadState = true;
    m_saveState = true;
    m_completerForLineEdit = false;
    m_completerMaxSize = 0;
    if (m_className == "QLineEdit") {
        for (int i = 0; i<widgetElement.childNodes().count(); i++) {
            QDomElement propertyElement = widgetElement.childNodes().at(i).toElement();
            if (propertyElement.attribute("name") == "GenerateCompleter") {
                for (int j = 0; j<propertyElement.childNodes().count(); j++) {
                    if (propertyElement.childNodes().at(j).toElement().tagName() == "bool" &&
                            propertyElement.childNodes().at(j).toElement().text() == "true") {
                        m_completerForLineEdit = true;
                    }
                }
            } else if (propertyElement.attribute("name") == "CompleterMaxCount") {
                for (int j = 0; j<propertyElement.childNodes().count(); j++) {
                    if (propertyElement.childNodes().at(j).toElement().tagName() == "number") {
                        m_completerMaxSize = propertyElement.childNodes().at(j).toElement().text().toInt();
                    }
                }
            }
        }
    }
}

/**
 * @brief WidgetSettings::saveMethod
 * @param settingsObjectName The QSetting instance name without the operand (. or ->)
 * @param settingsObjectOperand "." or "->" depending if the settingsObject is pointer or not
 * @return Returns a string containing the widget state save to the QSettings object
 */
void WidgetSettings::generateSaveMethod(KODE::Code *code, QString settingsObjectName, QString settingsObjectOperand,  QString sectionName)
{
    if (!m_saveState)
        return;

    QString accessor = QString("ui->%1").arg(m_widgetName);
    if (m_topLevel)
        accessor = "this";

    QString settingsObjectFull = settingsObjectName + settingsObjectOperand;
    // write begingroup if we are in a section
    if (!sectionName.isEmpty()) {
        code->addLine(QString("%1beginGroup(\"%2\");")
                      .arg(settingsObjectFull)
                      .arg(sectionName));
    }

    QString variantValue = getValueString();
    if (variantValue.isEmpty()) {
        qWarning() << "Unsupported widget type:" << m_className;
        return; // return with empty string for non savable widgets
    }

    if (m_className == "QLineEdit" && m_completerForLineEdit) {
        if (m_completerMaxSize == 0) {
            code->addLine("saveLineEditCompleter(" +accessor+ ", " + settingsObjectName + ");");
        } else {
            code->addLine("saveLineEditCompleter(" +accessor+ ", " + settingsObjectName + ", " + QString::number(m_completerMaxSize) + ");");
        }
    }

    code->addLine(QString("%1setValue(\"%2\", %3);").arg(settingsObjectFull).arg(m_keyName).arg(variantValue));
    // write endgroup if we are in a section
    if (!sectionName.isEmpty()) {
        code->addLine(QString("%1endGroup();")
                      .arg(settingsObjectFull));
    }
}

/**
 * @brief WidgetSettings::loadMethod
 * @param settingsObjectName The QSetting instance name without the operand (. or ->)
 * @param settingsObjectOperand "." or "->" depending if the settingsObject is pointer or not
 * @return Returns a string containing the method which will apply the state readed from the settings to the widget
 */
void WidgetSettings::generateLoadMethod(KODE::Code *code, QString settingsObjectName,  QString settingsObjectOperand, QString sectionName)
{
    if (!m_loadState)
        return;

    QString accessor = QString("ui->%1").arg(m_widgetName);
    if (m_topLevel)
        accessor = "this";

    QString settingsObjectFull = settingsObjectName + settingsObjectOperand;
    if (!sectionName.isEmpty()) {
        // write begingroup if we are in a section
        code->addLine(QString("%1beginGroup(\"%2\");")
                            .arg(settingsObjectFull)
                            .arg(sectionName));
    }

    if (m_className == "QCheckBox" ||
            m_className == "QRadioButton" ||
            m_className == "QPushButton") {
        code->addLine(QString("%1->setChecked(%2value(\"%3\").toBool());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QLineEdit") {
        if (m_completerMaxSize == 0) {
            code->addLine("loadLineEditCompleter(ui->" +m_widgetName+ ", " + settingsObjectName + ");");
        } else {
            code->addLine("loadLineEditCompleter(ui->" +m_widgetName+ ", " + settingsObjectName + ", " + QString::number(m_completerMaxSize) + ");");
        }
        code->addLine(QString("%1->setText(%2value(\"%3\").toString());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QPlainTextEdit") {
        code->addLine(QString("%1->setPlainText(%2value(\"%3\").toString());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QSpinBox" ||
             m_className == "QSlider" ||
             m_className == "QDial") {
        code->addLine(QString("%1->setValue(%2value(\"%3\").toInt());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if(m_className == "QDoubleSpinBox") {
        code->addLine(QString("%1->setValue(%2value(\"%3\").toDouble());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if(m_className == "QTextEdit") {
        code->addLine(QString("%1->setHtml(%2value(\"%3\").toString());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if(m_className == "QDateEdit") {
        code->addLine(QString("%1->setDate(%2value(\"%3\").toDate());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if(m_className == "QDateTimeEdit") {
        code->addLine(QString("%1->setDateTime(%2.alue(\"%3\").toDateTime());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if(m_className == "QTimeEdit") {
        code->addLine(QString("%1->setTime(%2value(\"%3\").toTime());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QComboBox" ||
             m_className == "QTabWidget" ||
             m_className == "QTabView") {
        code->addLine(QString("%1->setCurrentIndex(%2value(\"%3\").toInt());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QWidget" || m_className == "QDialog" || m_className == "QMainWindow") {
        code->addLine(QString("%1->restoreGeometry(%2value(\"%3\").toByteArray());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    } else if (m_className == "QPathEdit") {
        code->addLine(QString("%1->setPath(%2value(\"%3\", true).toString());")
                .arg(accessor)
                .arg(settingsObjectFull)
                .arg(m_keyName));
    }

    if (!sectionName.isEmpty()) {
        // write endgroup if we are in a section
        code->addLine(QString("%1endGroup();")
                            .arg(settingsObjectFull));
    }
}

bool WidgetSettings::getCompleterForLineEdit() const
{
    return m_completerForLineEdit;
}

bool WidgetSettings::getTopLevel() const
{
    return m_topLevel;
}

void WidgetSettings::setTopLevel(bool topLevel)
{
    m_topLevel = topLevel;
}

QString WidgetSettings::getValueString()
{
    QString accessor = QString("ui->%1").arg(m_widgetName);
    if (m_topLevel)
        accessor = "this";

    if (m_className == "QCheckBox" ||
            m_className == "QRadioButton" ||
            m_className == "QPushButton")
        return QString("%1->isChecked()").arg(accessor);
    else if (m_className == "QLineEdit")
        return QString("%1->text()").arg(accessor);
    else if(m_className == "QPlainTextEdit")
        return QString("%1->toPlainText()").arg(accessor);
    else if (m_className == "QTextEdit")
        return QString("%1->toHtml()").arg(accessor);
    else if (m_className == "QSpinBox" ||
             m_className == "QDoubleSpinBox" ||
             m_className == "QSlider" ||
             m_className == "QDial")
        return QString("%1->value()").arg(accessor);
    else if (m_className == "QDateEdit")
        return QString("%1->date()").arg(accessor);
    else if (m_className == "QDateTimeEdit")
        return QString("%1->dateTime()").arg(accessor);
    else if (m_className == "QTimeEdit")
        return QString("%1->time()").arg(accessor);
    else if (m_className == "QComboBox" ||
             m_className == "QTabWidget" ||
             m_className == "QTabView")
        return QString("%1->currentIndex()").arg(accessor);
    else if (m_className == "QWidget" || m_className == "QDialog" || m_className == "QMainWindow")
        return QString("%1->saveGeometry()").arg(accessor);
    else if (m_className == "QPathEdit")
        return QString("%1->path()").arg(accessor);
    return "";
}


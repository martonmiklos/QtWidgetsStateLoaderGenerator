#ifndef SETTINGSSAVERMODEL_H
#define SETTINGSSAVERMODEL_H

#include <QAbstractTableModel>
#include <QDomElement>

#include "uifileimporter.h"
#include "code.h"

class UIFileImporter;
/*
class WidgetClassUtil // TODO refactor more meaninful name
{
public:
    WidgetClassUtil(QString );
    QStringList supportedClasses;
};*/

class WidgetSettings
{
public:
    WidgetSettings(QDomElement widgetElement);
public:
    enum WidgetProjectState {
        PresentBoth,
        RemovedFromUI,
        AddedFromUI
    };

    QString className() const {return m_className;}
    QString widgetName() const {return m_widgetName;}
    void setWidgetName(QString widgetName) {m_widgetName = widgetName;}

    QString section() const {return m_section;}
    void setSection(QString section) {m_section = section;}

    WidgetProjectState state() const {return m_state;}
    void setState(WidgetProjectState state) {m_state = state;}

    bool saveState() const {return m_saveState;}
    void setSaveState(bool saveState) {m_saveState = saveState;}

    bool loadState() const {return m_loadState;}
    void setLoadState(bool loadState) {m_loadState = loadState;}

    void generateSaveMethod(KODE::Code *code, QString settingsObjectName, QString settingsObjectOperand, QString sectionName = "");
    void generateLoadMethod(KODE::Code *code, QString settingsObjectName, QString settingsObjectOperand, QString sectionName = "");

    QString keyName() const {return m_keyName;}
    void setKeyName(QString keyName) {m_keyName = keyName;}


    bool getCompleterForLineEdit() const;

private:
    bool m_saveState;
    bool m_loadState;
    WidgetProjectState m_state;
    QString m_section;
    QString m_className;
    QString m_widgetName;
    QString m_keyName;

    bool m_completerForLineEdit;
    int m_completerMaxSize;

    QString getValueString();
};

class SettingsSaverModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SettingsSaverModel(QObject *parent = 0);


    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    bool saveToFile(QString filePath,
                    QString uiImplementationHeader,
                    QString uiDeclarationHeader,
                    QString saveMethod = "saveWidgetsState",
                    QString loadMethod = "loadWidgetsState",
                    bool passSettingAsArgument = false,
                    QString argument = "");
    QString generate(QString filePath,
                     QString uiImplementationHeader,
                     QString uiDeclarationHeader,
                     bool passSettingAsArgument = false,
                     QString argument = "");
    bool importUIFile(QString filePath);

    QString error() const;
    void setError(const QString &error);

private:
    QList<WidgetSettings*> m_elements;
    UIFileImporter *m_uiImporter;
    QString m_error;
signals:

public slots:

};

#endif // SETTINGSSAVERMODEL_H

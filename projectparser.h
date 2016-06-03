#ifndef PROJECTPARSER_H
#define PROJECTPARSER_H

#include <QString>
#include <QObject>

#include "settingssavermodel.h"

class ProjectParser : public QObject
{
public:
    ProjectParser(QObject *parent = NULL);

    bool parseProject(QString projectPath);
    bool importUIFile(QString uiFilePath);
    bool generate();

    QString error() const;

private:
    SettingsSaverModel *m_model;
    QString m_uiFilePath, m_outputFile, m_uiHeaderPath, m_uiImplementationHeader, m_saveMethod, m_loadMethod, m_argument, m_error;
    bool m_passArgument;
};

#endif // PROJECTPARSER_H

#ifndef UIFILEIMPORTER_H
#define UIFILEIMPORTER_H

#include <QString>
#include <QList>


#include "settingssavermodel.h"

class WidgetSettings;

class UIFileImporter
{
public:
    UIFileImporter();
    QList<WidgetSettings *> importUIFile(QString uiPath, bool *ok = 0);
};

#endif // UIFILEIMPORTER_H

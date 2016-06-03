#include "uifileimporter.h"

#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

UIFileImporter::UIFileImporter()
{
}

QList<WidgetSettings*> UIFileImporter::importUIFile(QString uiPath, bool *ok)
{
    QList<WidgetSettings*> ret;
    if (uiPath != "") {
        QDomDocument doc("mydocument");
        QFile file(uiPath);
        if (!file.open(QIODevice::ReadOnly)) {
            if (ok != 0)
                *ok = false;
            return ret;
        }

        if (!doc.setContent(&file)) {
            file.close();
            if (ok != 0)
                *ok = false;
            return ret;
        }
        file.close();

        QDomElement docElem = doc.documentElement();
        QDomNodeList widgetNodes = docElem.elementsByTagName("widget");
        for (int i = 0; i < widgetNodes.size(); i++) {
            QDomElement element = widgetNodes.at(i).toElement();
            bool add = true;
            // add QPushButtons only if has checkable attribute
            if (element.attribute("class") == "QPushButton") {
                add = false;
                for (int j = 0; j<element.childNodes().count(); j++) {
                    QDomElement propertyElement = element.childNodes().at(j).toElement();
                    bool checkableTrue = false;

                    for (int k = 0; k<propertyElement.childNodes().count(); k++) {
                        if (propertyElement.childNodes().at(k).isText() && propertyElement.childNodes().at(k).nodeValue() == "true") {
                            checkableTrue = true;
                            break;
                        }
                    }

                    if (propertyElement.attribute("name") == "checkable" && checkableTrue) {
                        add = true;
                        break;
                    }
                }
            } else if (element.attribute("class") == "QWidget" && element.parentNode().toElement().attribute("class") == "QTabWidget") {
                // do not add savegeometry for tabs
                add = false;
            }

            if (add) {
                WidgetSettings *widget = new WidgetSettings(element);
                ret.append(widget);
            }
        }
    }
    *ok = true;
    return ret;
}

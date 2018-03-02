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
    bool first = true;
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
            // loop on widgets in the UI file
            QDomElement widgetElement = widgetNodes.at(i).toElement();
            bool add = true;

            for (int j = 0; j<widgetElement.childNodes().count(); j++) {
                // loop on the widget's properties
                QDomElement property = widgetElement.childNodes().at(j).toElement();
                if (property.attribute("name") == "enabled") {
                    for (int k = 0; k<property.childNodes().count(); k++) {
                        if (property.childNodes().at(k).toElement().tagName() == "bool") {
                            if (property.childNodes().at(k).toElement().text() == "false")
                                add = false; // do not add loader/saver for disabled widgets
                            break;
                        }
                    }
                }
            }
            if (add) {
                // add QPushButtons only if has checkable attribute
                if (widgetElement.attribute("class") == "QPushButton") {
                    add = false;
                    for (int j = 0; j<widgetElement.childNodes().count(); j++) {
                        QDomElement propertyElement = widgetElement.childNodes().at(j).toElement();
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
                } else if (widgetElement.attribute("class") == "QWidget" && widgetElement.parentNode().toElement().attribute("class") == "QTabWidget") {
                    // do not add savegeometry for tabs
                    add = false;
                }
            }

            if (add) {
                WidgetSettings *widget = new WidgetSettings(widgetElement);
                if (first)
                    widget->setTopLevel(true);
                ret.append(widget);
            }
            first = false;
        }
    }
    *ok = true;
    return ret;
}

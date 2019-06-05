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
        QDomNodeList customWidgetNodes = docElem.elementsByTagName("customwidgets");
        QDomNode customWidgetNode = customWidgetNodes.at(0);
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
                if (widgetElement.attribute("class") == "QPushButton" ||
                    widgetElement.attribute("class") == "QToolButton") {
                    add = false;
                    for (int j = 0; j<widgetElement.childNodes().count(); j++) {
                        QDomElement propertyElement = widgetElement.childNodes().at(j).toElement();
                        if (propertyElement.attribute("name") == "checkable") {
                            for (int k = 0; k<propertyElement.childNodes().count(); k++) {
                                if (propertyElement.childNodes().at(k).toElement().tagName() == "bool"
                                        && propertyElement.childNodes().at(k).toElement().text() == "true") {
                                    add = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                } else if (widgetElement.attribute("class") == "QWidget" && widgetElement.parentNode().toElement().attribute("class") == "QTabWidget") {
                    // do not add savegeometry for tabs
                    add = false;
                } else if (widgetElement.attribute("class") == "QPlainTextEdit") {
                    for (int j = 0; j<widgetElement.childNodes().count(); j++) {
                        QDomElement propertyElement = widgetElement.childNodes().at(j).toElement();
                        if (propertyElement.attribute("name") == "readOnly") {
                            for (int k = 0; k<propertyElement.childNodes().count(); k++) {
                                if (propertyElement.childNodes().at(k).toElement().tagName() == "bool"
                                        && propertyElement.childNodes().at(k).toElement().text() == "true") {
                                    add = false;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }

            if (add) {
                WidgetSettings *widget = new WidgetSettings(widgetElement, customWidgetNode);
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

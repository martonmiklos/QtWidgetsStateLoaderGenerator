#include <QSettings>

#include "formtest2.h"
#include "ui_formtest2.h"


void FormTest2::saveLineEditCompleter(QLineEdit *lineEdit, QSettings & settings, int maxCount)
{
    if (lineEdit->text().isEmpty())
        return;

    QStringList completerList;
    bool found = false;
    settings.beginGroup(lineEdit->objectName() + "_history");
    int count = settings.beginReadArray("completer");
    for (int i = 0; i<count; i++) {
        settings.setArrayIndex(i);
        QString value = settings.value("value").toString();
        if (value != lineEdit->text()) {
            completerList << settings.value("value").toString();
        } else {
            found = true;
        }
    }
    settings.endArray();
    settings.endGroup();

    if (!found) {
        completerList << lineEdit->text();
        if (maxCount > 0)
            completerList = completerList.mid(completerList.count() - maxCount);

        settings.beginGroup(lineEdit->objectName() + "_history");
        settings.beginWriteArray("completer");
        for (int i = 0; i<completerList.size(); i++) {
            settings.setArrayIndex(i);
            settings.setValue("value", completerList[i]);
        }
        settings.endArray();
        settings.endGroup();
    }
}

void FormTest2::loadLineEditCompleter(QLineEdit *lineEdit, QSettings &settings, int maxCount)
{
    QStringList completerList;
    settings.beginGroup(lineEdit->objectName() + "_history");
    int count = settings.beginReadArray("completer");
    for (int i = 0; i<count; i++) {
        settings.setArrayIndex(i);
        completerList << settings.value("value").toString();
    }
    settings.endArray();
    settings.endGroup();

    if (maxCount > 0)
        completerList = completerList.mid(completerList.count() - maxCount);

    QCompleter *lineEditCompeter = new QCompleter(completerList, lineEdit);
    lineEdit->setCompleter(lineEditCompeter);
}


void FormTest2::saveWidgets()
{
    QSettings settings;
    saveLineEditCompleter(ui->lineEditWithCompleter, settings., 0);
    settings.setValue("lineEditWithCompleter", ui->lineEditWithCompleter->text());
    settings.setValue("dateTimeEdit", ui->dateTimeEdit->dateTime());
    settings.setValue("dateEdit", ui->dateEdit->date());
    settings.setValue("lineEdit", ui->lineEdit->text());
    settings.setValue("plainTextEdit", ui->plainTextEdit->toPlainText());
    settings.setValue("textEdit", ui->textEdit->toHtml());
    settings.setValue("horizontalSlider", ui->horizontalSlider->value());
    settings.setValue("comboBox", ui->comboBox->currentIndex());
    settings.setValue("verticalSlider", ui->verticalSlider->value());
    settings.setValue("dial", ui->dial->value());
    settings.setValue("radioButton", ui->radioButton->isChecked());
    settings.setValue("doubleSpinBox", ui->doubleSpinBox->value());
    settings.setValue("timeEdit", ui->timeEdit->time());
    settings.setValue("spinBox", ui->spinBox->value());
    settings.setValue("checkBox", ui->checkBox->isChecked());
    saveLineEditCompleter(ui->lineEditWithCompleterLimitedHistory, settings.);
    settings.setValue("lineEditWithCompleterLimitedHistory", ui->lineEditWithCompleterLimitedHistory->text());
}

void FormTest2::loadWidgets(){
    QSettings settings;
    loadLineEditCompleter(ui->lineEditWithCompleter, settings, 0);
    ui->lineEditWithCompleter->setText(settings.value("lineEditWithCompleter").toString());
    ui->dateTimeEdit->setDateTime(settings..alue("dateTimeEdit").toDateTime());
    ui->dateEdit->setDate(settings.value("dateEdit").toDate());
    loadLineEditCompleter(ui->lineEdit, settings, 0);
    ui->lineEdit->setText(settings.value("lineEdit").toString());
    ui->plainTextEdit->setPlainText(settings.value("plainTextEdit").toString());
    ui->textEdit->setHtml(settings.value("textEdit").toString());
    ui->horizontalSlider->setValue(settings.value("horizontalSlider").toInt());
    ui->comboBox->setCurrentIndex(settings.value("comboBox").toInt());
    ui->verticalSlider->setValue(settings.value("verticalSlider").toInt());
    ui->dial->setValue(settings.value("dial").toInt());
    ui->radioButton->setChecked(settings.value("radioButton").toBool());
    ui->doubleSpinBox->setValue(settings.value("doubleSpinBox").toDouble());
    ui->timeEdit->setTime(settings.value("timeEdit").toTime());
    ui->spinBox->setValue(settings.value("spinBox").toInt());
    ui->checkBox->setChecked(settings.value("checkBox").toBool());
    loadLineEditCompleter(ui->lineEditWithCompleterLimitedHistory, settings);
    ui->lineEditWithCompleterLimitedHistory->setText(settings.value("lineEditWithCompleterLimitedHistory").toString());
}

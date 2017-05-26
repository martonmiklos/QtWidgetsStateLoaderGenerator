#include <QSettings>

#include "formtest2.h"
#include "ui_formtest2.h"


void FormTest2::saveWidgets()
{
    QSettings settings;
    settings.setValue("FormTest2", ui->FormTest2->saveGeometry());
    if (!ui->lineEditWithCompleter->text().isEmpty()) {
        QStringList completerList;
        bool found = false;
        for (int i = 0; i<settings.beginReadArray("lineEditWithCompleter_history"); i++) {
            settings.setArrayIndex(i);
            QString value = settings.value("value").toString();
            if (value == ui->lineEditWithCompleter->text()) {
                found = true;
                completerList << lineEditWithCompletervalue("value").toString();
            }
            settings.endArray();

            if (!found) {
                completeList << ui->lineEditWithCompleter->text();
                settings.beginWriteArray("lineEditWithCompleter_history");
                for (int i = 0; i<completerList.size(); i++) {
                    settings.setArrayIndex(i);
                    settings.setValue("value", completeList[i]);
                }
                settings.endArray();
            }
        }
    }

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
    if (!ui->lineEditWithCompleterLimitedHistory->text().isEmpty()) {
        QStringList completerList;
        bool found = false;
        for (int i = 0; i<settings.beginReadArray("lineEditWithCompleterLimitedHistory_history"); i++) {
            settings.setArrayIndex(i);
            QString value = settings.value("value").toString();
            if (value == ui->lineEditWithCompleterLimitedHistory->text()) {
                found = true;
                completerList << lineEditWithCompleterLimitedHistoryvalue("value").toString();
            }
            settings.endArray();

            if (!found) {
                completeList << ui->lineEditWithCompleterLimitedHistory->text();
                settings.beginWriteArray("lineEditWithCompleterLimitedHistory_history");
                for (int i = 0; i<completerList.size(); i++) {
                    settings.setArrayIndex(i);
                    settings.setValue("value", completeList[i]);
                }
                settings.endArray();
            }
        }
    }

    settings.setValue("lineEditWithCompleterLimitedHistory", ui->lineEditWithCompleterLimitedHistory->text());
}

void FormTest2::loadWidgets(){
    QSettings settings;
    ui->FormTest2->restoreGeometry(settings.value("FormTest2").toByteArray());
    ui->lineEditWithCompleter->setText(settings.value("lineEditWithCompleter").toString());
    ui->dateTimeEdit->setDateTime(settings..alue("dateTimeEdit").toDateTime());
    ui->dateEdit->setDate(settings.value("dateEdit").toDate());
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
    ui->lineEditWithCompleterLimitedHistory->setText(settings.value("lineEditWithCompleterLimitedHistory").toString());
}

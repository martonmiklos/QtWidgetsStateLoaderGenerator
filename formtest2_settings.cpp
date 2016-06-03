#include <QSettings>

#include "formtest2.h"
#include "ui_formtest2.h"

void FormTest2::saveWidgetStates()
{
	QSettings settings;
	settings.setValue("pushButton", ui->pushButton->isChecked());
	settings.setValue("dateTimeEdit", ui->dateTimeEdit->dateTime());
	settings.setValue("dateEdit", ui->dateEdit->date());
	settings.setValue("lineEdit", ui->lineEdit->text());
	settings.setValue("plainTextEdit", ui->plainTextEdit->toPlainText());
	settings.setValue("textEdit", ui->textEdit->toHtml());
	settings.setValue("horizontalSlider", ui->horizontalSlider->value());
	settings.setValue("verticalSlider", ui->verticalSlider->value());
	settings.setValue("dial", ui->dial->value());
	settings.setValue("radioButton", ui->radioButton->isChecked());
	settings.setValue("timeEdit", ui->timeEdit->time());
	settings.setValue("spinBox", ui->spinBox->value());
	settings.setValue("doubleSpinBox", ui->doubleSpinBox->value());
	settings.setValue("checkBox", ui->checkBox->isChecked());
}

void FormTest2::loadWidgetStates()
{
	QSettings settings;
	ui->pushButton->setChecked(settings.value("pushButton").toBool());
	ui->dateTimeEdit->setDateTime(settings.value("dateTimeEdit").toDateTime());
	ui->dateEdit->setDate(settings.value("dateEdit").toDate());
	ui->lineEdit->setText(settings.value("lineEdit").toString());
	ui->plainTextEdit->setPlainText(settings.value("plainTextEdit").toString());
	ui->textEdit->setHtml(settings.value("textEdit").toString());
	ui->horizontalSlider->setValue(settings.value("horizontalSlider").toInt());
	ui->verticalSlider->setValue(settings.value("verticalSlider").toInt());
	ui->dial->setValue(settings.value("dial").toInt());
	ui->radioButton->setChecked(settings.value("radioButton").toBool());
	ui->timeEdit->setTime(settings.value("timeEdit").toTime());
	ui->spinBox->setValue(settings.value("spinBox").toInt());
	ui->doubleSpinBox->setValue(settings.value("doubleSpinBox").toDouble());
	ui->checkBox->setChecked(settings.value("checkBox").toBool());
}


#include <QSettings>

#include "widgetsettingssaver.h"
#include "ui_widgetsettingssaver.h"

void WidgetSettingsSaver::saveSettings(QSettings *settings)
{
	settings->setValue("checkBoxPassArgument", ui->checkBoxPassArgument->isChecked());
	settings->setValue("lineEditOutputFileFakk", ui->lineEditOutputFile->text());
	settings->setValue("lineEditUI_File", ui->lineEditUI_File->text());
	settings->setValue("uIImplementationHeaderFileLineEdit", ui->uIImplementationHeaderFileLineEdit->text());
	settings->setValue("lineEditSaveMethod", ui->lineEditSaveMethod->text());
	settings->setValue("lineEditLoadMethod", ui->lineEditLoadMethod->text());
	settings->setValue("lineEditArgument", ui->lineEditArgument->text());
}

void WidgetSettingsSaver::loadSettings(QSettings *settings)
{
	ui->WidgetSettingsSaver->restoreGeometry(settings->value("WidgetSettingsSaver").toByteArray());
	ui->checkBoxPassArgument->setChecked(settings->value("checkBoxPassArgument").toBool());
	ui->lineEditOutputFile->setText(settings->value("lineEditOutputFileFakk").toString());
	ui->lineEditUI_File->setText(settings->value("lineEditUI_File").toString());
	ui->uIImplementationHeaderFileLineEdit->setText(settings->value("uIImplementationHeaderFileLineEdit").toString());
	ui->lineEditSaveMethod->setText(settings->value("lineEditSaveMethod").toString());
	ui->lineEditLoadMethod->setText(settings->value("lineEditLoadMethod").toString());
	ui->lineEditArgument->setText(settings->value("lineEditArgument").toString());
}


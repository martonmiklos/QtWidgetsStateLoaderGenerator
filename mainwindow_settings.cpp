#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::saveWidgetStates()
{
	QSettings settings;
	settings.setValue("centralWidget", ui->centralWidget->saveGeometry());
	settings.setValue("tabWidget", ui->tabWidget->currentIndex());
}

void MainWindow::loadWidgetStates()
{
	QSettings settings;
	ui->centralWidget->restoreGeometry(settings.value("centralWidget").toByteArray());
	ui->tabWidget->setCurrentIndex(settings.value("tabWidget").toInt());
}


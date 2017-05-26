#include "formtest2.h"
#include "ui_formtest2.h"
#include <QCloseEvent>

FormTest2::FormTest2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTest2)
{
    ui->setupUi(this);
    loadWidgetStates();
}

FormTest2::~FormTest2()
{
    saveWidgetStates();
    delete ui;
}

#ifndef FORMTEST2_H
#define FORMTEST2_H

#include <QWidget>

namespace Ui {
class FormTest2;
}

class FormTest2 : public QWidget
{
    Q_OBJECT

public:
    explicit FormTest2(QWidget *parent = 0);
    ~FormTest2();
    void closeEvent(QCloseEvent *event);
private:
    Ui::FormTest2 *ui;
    void saveWidgetStates();
    void loadWidgetStates();
};

#endif // FORMTEST2_H

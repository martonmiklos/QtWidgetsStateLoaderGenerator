#ifndef FORMTEST2_H
#define FORMTEST2_H

#include <QWidget>
#include <QLineEdit>
#include <QSettings>

namespace Ui {
class FormTest2;
}

class FormTest2 : public QWidget
{
    Q_OBJECT

public:
    explicit FormTest2(QWidget *parent = 0);
    ~FormTest2();
private:
    Ui::FormTest2 *ui;
    void saveWidgetStates();
    void loadWidgetStates();
    void saveLineEditCompleter(QLineEdit *lineEdit, QSettings &settings, int maxCount = 0);
    void loadLineEditCompleter(QLineEdit *lineEdit, QSettings &settings, int maxCount = 0);
};

#endif // FORMTEST2_H

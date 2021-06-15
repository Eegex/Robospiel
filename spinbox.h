#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
    Q_OBJECT
public:
    SpinBox(QWidget* paren=nullptr);
public slots:
    void keyPressEvent(QKeyEvent *event);
signals:
    void returnPressed();
};

#endif // SPINBOX_H

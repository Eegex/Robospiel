#ifndef KEYINPUT_H
#define KEYINPUT_H

#include <QComboBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

class KeyInput : public QWidget
{
    Q_OBJECT

    QVBoxLayout* layout;
    QComboBox* selection;
    QPushButton* deleteBtn;

public:
    explicit KeyInput(Qt::Key key, QWidget *parent = nullptr);

signals:
    void deletedKey(QString currentSelection);
    void changedKey(QString currentSelection);

};

#endif // KEYINPUT_H

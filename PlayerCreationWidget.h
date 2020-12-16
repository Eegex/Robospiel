#ifndef PLAYERCREATIONWIDGET_H
#define PLAYERCREATIONWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QLineEdit>

struct user{
    QString name;
    QColor colour;
};

class PlayerCreationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerCreationWidget(QWidget *parent = nullptr);

private:
    QGridLayout * lay = new QGridLayout(this);
    QPushButton * addPlayerBtn = new QPushButton("Spieler hinzufügen", this); //Translate-Funktion
    QPushButton * addColourBtn = new QPushButton("Farbe hinzufügen", this);
    QColor * playerColor = new QColor();
    QColorDialog * playerColourPicker = new QColorDialog();
    QLineEdit * playerNamePicker = new QLineEdit();
    unsigned int numOfPlayers = 0;

public slots:
    void addPlayer();
    void addColour();
    void resetButtonColour();
signals:
    void playerAdded(struct user * newUser); //Argument: Nutzer
};
#endif // PLAYERCREATIONWIDGET_H

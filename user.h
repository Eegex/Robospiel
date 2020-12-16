#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>
#include <QColor>

class User : public QObject
{
    Q_OBJECT
public:
    User(QString name, QColor color, QObject *parent);

    QString getName();
    QColor getColor();
    int getTip();
    int getPoints();

    void setName(QString n);
    void setColor(QColor c);
    void setTip(int t);
    void addPoints(int p);

private:
    QString name;
    QColor color;
    int tip;
    int points = 0;

signals:

};

#endif // USER_H

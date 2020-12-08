#ifndef USER_H
#define USER_H

#include <QObject>
#include <string>
#include <QColor>

class User : public QObject
{
    Q_OBJECT
public:
    User(std::string name, QColor color, QObject *parent);

    std::string getName();
    QColor getColor();
    int getTip();
    int getPoints();

    void setName(std::string n);
    void setColor(QColor c);
    void setTip(int t);
    void addPoints(int p);

private:
    std::string name;
    QColor color;
    int tip;
    int points = 0;

signals:

};

#endif // USER_H

#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

class SpinBox : public QSpinBox
{
	Q_OBJECT
public:
	SpinBox(QWidget* paren=nullptr);
	void setSelectAllOnFocus(bool newSelectAllOnFocus);

	bool getSelectAllOnFocus() const;

public slots:
	void focusInEvent(QFocusEvent * event);
	void keyPressEvent(QKeyEvent *event);
private:
	bool selectAllOnFocus = false;
signals:
	void returnPressed();
};

#endif // SPINBOX_H

#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QModelIndex>
#include <QPoint>
#include <QStyledItemDelegate>
#include <QWidget>

class ButtonDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ButtonDelegate(QWidget * parent = nullptr);
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

	void resetState();
public slots:
	void handleClick(const QModelIndex& index);
	void handleHover(const QModelIndex& index);
signals:
	void buttonClicked(QModelIndex index);
	void buttonHovered(QModelIndex index);

protected:
	QPoint clicked = QPoint(-1,-1);
private:
	virtual int hasButton(QPoint p) const =0; //0=no button, no text, 1=text, 2=button
	virtual bool isButtonDisabled(QPoint p) const=0;
	virtual QSize getCustomSizeHint(QPoint p) const=0;

	QPoint hovered = QPoint(-1,-1);
	QPoint delay = QPoint(-1,-1);
	char smaller = 4;
};

#endif // BUTTONDELEGATE_H

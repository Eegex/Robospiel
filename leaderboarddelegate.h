#ifndef LEADERBOARDDELEGATE_H
#define LEADERBOARDDELEGATE_H

#include <QStyledItemDelegate>
#include <QWidget>

class LeaderboardDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	LeaderboardDelegate(QSize size, QWidget * parent = nullptr);
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void setButtonVisible(bool visible);

public slots:
	void updateSizeHint(int logicalIndex, int oldSize, int newSize);
	void userHovered(const QModelIndex & index);
	void userClicked(const QModelIndex & index);
	void hideClickedButton();
private:
	int hovered = -1;
	int clicked = -1;
	QSize customSizeHint = {80,20};
	bool buttonVisible = false;
	bool delay = false;
	char smaller = 4;
signals:
	void userBtnClicked(QModelIndex & index);

};

#endif // LEADERBOARDDELEGATE_H

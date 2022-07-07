#ifndef KEYINPUTBLOCK_H
#define KEYINPUTBLOCK_H

#include "keyinputrecorder.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class KeyInputBlock : public QWidget
{
	Q_OBJECT

public:
	explicit KeyInputBlock(QWidget *parent = nullptr);

	bool hasKey();
	void reset();
	Qt::Key getKey();

private:
	KeyInputRecorder* input = nullptr;
signals:
	void addMapping();

};

#endif // KEYINPUTBLOCK_H

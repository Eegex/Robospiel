#include "keyinputblock.h"

#include <QMetaEnum>
#include <QKeyEvent>

//contains input line and add button
KeyInputBlock::KeyInputBlock(QWidget *parent) : QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	input = new KeyInputRecorder(this);
	QPushButton* btnAddKey = new QPushButton(tr("Add"), this);
	connect(btnAddKey, &QPushButton::clicked, this, [=](){
		emit addMapping();
	});
	btnAddKey->setFixedWidth(input->getInputWidth());

	layout->addWidget(input);
	layout->addWidget(btnAddKey);
	//layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

bool KeyInputBlock::hasKey()
{
	return input->getKey()!=Qt::Key_unknown && input->getKey()!=Qt::Key(0);
}

Qt::Key KeyInputBlock::getKey()
{
	return input->getKey();
}

void KeyInputBlock::reset()
{
	input->reset();
}







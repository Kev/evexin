/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtMainWindow.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QImage>
#include <QPixmap>
#include <QPushButton>

#include <Swiften/Base/foreach.h>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>

#include <Eve-Xin/QtUI/QtCharacterPane.h>

namespace EveXin {

QtMainWindow::QtMainWindow(boost::shared_ptr<DataController> dataController) {
	dataController_ = dataController;
	QWidget* mainWidget = new QWidget();
	setCentralWidget(mainWidget);
	QBoxLayout* rightLeftLayout = new QBoxLayout(QBoxLayout::LeftToRight, mainWidget);
	QBoxLayout* characterLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	characterComboBox_ = new QComboBox(this);
	characterLayout->addWidget(characterComboBox_);
	characterPane_ = new QtCharacterPane(this);
	characterLayout->addWidget(characterPane_);
	rightLeftLayout->addLayout(characterLayout);
	//connect(ui->addKey, SIGNAL(clicked()), this, SLOT(handleAddKey()));
	connect(characterComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCharacterSelected(int)));
	handleCharacterListUpdated();
	dataController_->onCharacterDataChanged.connect(boost::bind(&QtMainWindow::handleCharacterDataUpdated, this, _1));
}

QtMainWindow::~QtMainWindow() {
	
}

void QtMainWindow::handleAddKey() {
	//dataController_->addAPIKey(Q2PSTRING(ui->keyID->text()), Q2PSTRING(ui->verID->text()));
}

void QtMainWindow::handleCharacterListUpdated() {
	int previous = characterComboBox_->currentIndex();
	characterComboBox_->clear();
	foreach (const std::string& id, dataController_->getCharacters()) {
		characterComboBox_->addItem(P2QSTRING(dataController_->getCharacter(id)->getName()), QVariant(P2QSTRING(id)));
	}
	characterComboBox_->setCurrentIndex(previous);
	if ((characterComboBox_->currentIndex() < 0) && !(characterComboBox_->count() == 0)) {
		characterComboBox_->setCurrentIndex(0);
	}
	handleCharacterSelected(characterComboBox_->currentIndex());
}

Character::ref QtMainWindow::selectedCharacter() {
	Character::ref character;
	int index = characterComboBox_->currentIndex();
	if (index == -1) {
		return character;
	}
	std::string id = Q2PSTRING(characterComboBox_->itemData(index).toString());
	return dataController_->getCharacter(id);
}

void QtMainWindow::handleCharacterDataUpdated(const std::string& id) {
	Character::ref character = selectedCharacter();
	if (!character) {
		return;
	}
	characterPane_->setCharacter(character);
}

void QtMainWindow::handleCharacterSelected(int index) {
	Character::ref character = selectedCharacter();
	if (!character) {
		return;
	}
	handleCharacterDataUpdated(character->getID());
}

}

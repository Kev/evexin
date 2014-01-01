/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtMainWindow.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <QAction>
#include <QBoxLayout>
#include <QComboBox>
#include <QImage>
#include <QMenu>
#include <QMenuBar>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QTreeView>

#include <Swiften/Base/foreach.h>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>

#include <Eve-Xin/QtUI/QtAPIKeyWindow.h>
#include <Eve-Xin/QtUI/QtCharacterPane.h>
#include <Eve-Xin/QtUI/QtSkillDelegate.h>
#include <Eve-Xin/QtUI/QtSkillModel.h>
#include <Eve-Xin/QtUI/QtSkillPlannerWidget.h>

namespace EveXin {

QtMainWindow::QtMainWindow(boost::shared_ptr<DataController> dataController) {
	dataController_ = dataController;
	apiWindow_ = NULL;
	QWidget* mainWidget = new QWidget();
	setCentralWidget(mainWidget); // Takes ownership
	QBoxLayout* rightLeftLayout = new QBoxLayout(QBoxLayout::LeftToRight, mainWidget);

	characterComboBox_ = new QComboBox(this);
	rightLeftLayout->addWidget(characterComboBox_);

	QTabWidget* tabs = new QTabWidget(this);
	rightLeftLayout->addWidget(tabs);

	QWidget* characterTab = new QWidget(this);
	tabs->addTab(characterTab, "Character Sheet");
	QBoxLayout* characterLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	characterTab->setLayout(characterLayout);
	
	characterPane_ = new QtCharacterPane(this);
	characterLayout->addWidget(characterPane_);
	rightLeftLayout->addLayout(characterLayout);

	skillPane_ = new QTreeView(this);
	skillModel_ = boost::make_shared<QtSkillModel>();
	skillPane_->setModel(skillModel_.get());
	skillPane_->setUniformRowHeights(false);
	skillPane_->setHeaderHidden(true);
	QtSkillDelegate* delegate = new QtSkillDelegate(this);
	skillPane_->setItemDelegate(delegate);
	characterLayout->addWidget(skillPane_);

	skillPlannerWidget_ = new QtSkillPlannerWidget(dataController, this);
	QBoxLayout* skillPlanLayout = new QBoxLayout(QBoxLayout::LeftToRight);	
	skillPlannerWidget_->setLayout(skillPlanLayout);
	tabs->addTab(skillPlannerWidget_, "Skill Plan");

	connect(characterComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCharacterSelected(int)));
	handleCharacterListUpdated();
	dataController_->onCharacterDataChanged.connect(boost::bind(&QtMainWindow::handleCharacterDataUpdated, this, _1));
	createMenus();
}

QtMainWindow::~QtMainWindow() {
	delete apiWindow_;
}

void QtMainWindow::createMenus() {
	QMenu* menu = menuBar()->addMenu("General");
	QAction* editAPIKeyAction = menu->addAction("Edit API Keys");
	connect(editAPIKeyAction, SIGNAL(triggered()), this, SLOT(handleEditAPIKeys()));
}

void QtMainWindow::handleEditAPIKeys() {
	if (!apiWindow_) {
		apiWindow_ = new QtAPIKeyWindow(dataController_);
	}
	apiWindow_->show();
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
	skillModel_->setRoot(character->getKnownSkills());
}

void QtMainWindow::handleCharacterSelected(int index) {
	Character::ref character = selectedCharacter();
	if (!character) {
		return;
	}
	handleCharacterDataUpdated(character->getID());
	skillPlannerWidget_->setCharacter(character);
}

}

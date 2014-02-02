/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtMainWindow.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <QAction>
#include <QBoxLayout>
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
#include <Eve-Xin/QtUI/QtCharacterList.h>
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

	characterList_ = new QtCharacterList(this);
	rightLeftLayout->addWidget(characterList_);

	QTabWidget* tabs = new QTabWidget(this);
	rightLeftLayout->addWidget(tabs);

	QWidget* characterTab = new QWidget(this);
	tabs->addTab(characterTab, "Character Sheet");
	QBoxLayout* characterLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	characterTab->setLayout(characterLayout);
	
	characterPane_ = new QtCharacterPane(this);
	characterLayout->addWidget(characterPane_);

	skillPane_ = new QTreeView(this);
	skillModel_ = boost::make_shared<QtSkillModel>();
	skillPane_->setModel(skillModel_.get());
	skillPane_->setUniformRowHeights(false);
	skillPane_->setHeaderHidden(true);
	QtSkillDelegate* delegate = new QtSkillDelegate(this);
	skillPane_->setItemDelegate(delegate);
	characterLayout->addWidget(skillPane_);

	skillPlannerWidget_ = new QtSkillPlannerWidget(dataController, this);
	tabs->addTab(skillPlannerWidget_, "Skill Plan");

	characterList_->onCharacterSelected.connect(boost::bind(&QtMainWindow::handleCharacterSelected, this, _1));
	handleCharacterListUpdated();
	dataController_->onCharacterDataChanged.connect(boost::bind(&QtMainWindow::handleCharacterDataUpdated, this, _1));
	dataController_->onCharacterListChanged.connect(boost::bind(&QtMainWindow::handleCharacterListUpdated, this));
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
	auto characterIDs = dataController_->getCharacters();
	std::vector<Character::ref> characters;
	foreach (const std::string id, characterIDs) {
		characters.push_back(dataController_->getCharacter(id));
	}
	std::sort(characters.begin(), characters.end(), [](Character::ref a, Character::ref b){return a->getName() < b->getName();});
	characterList_->setCharacters(characters);
}

void QtMainWindow::handleCharacterDataUpdated(const std::string& /*id*/) {
	Character::ref character = characterList_->getCurrentCharacter();
	if (!character) {
		return;
	}
	characterPane_->setCharacter(character);
	skillModel_->setRoot(character->getKnownSkills());
}

void QtMainWindow::handleCharacterSelected(Character::ref character) {
	if (!character) {
		return;
	}
	handleCharacterDataUpdated(character->getID());
	skillPlannerWidget_->setCharacter(character);
}

}

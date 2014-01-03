/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtSkillPlannerWidget.h>

#include <QBoxLayout>
#include <QTreeView>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>
#include <Eve-Xin/Controllers/SkillTree.h>

#include <Eve-Xin/QtUI/QtSkillModel.h>
#include <Eve-Xin/QtUI/QtSkillDelegate.h>

namespace EveXin {

QtSkillPlannerWidget::QtSkillPlannerWidget(boost::shared_ptr<DataController> dataController, QWidget* parent) : QWidget(parent), dataController_(dataController) {
	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
	QTreeView* allSkillsWidget_ = new QTreeView(this);
	mainLayout->addWidget(allSkillsWidget_);
	allSkillsModel_ = new QtSkillModel();
	QtSkillDelegate* allSkillsDelegate = new QtSkillDelegate(this);
	allSkillsWidget_->setItemDelegate(allSkillsDelegate);
	allSkillsWidget_->setModel(allSkillsModel_);
	allSkillsWidget_->setUniformRowHeights(false);
	allSkillsWidget_->setHeaderHidden(true);
	QTreeView* plainWidget_ = new QTreeView(this);
	mainLayout->addWidget(plainWidget_);

}

QtSkillPlannerWidget::~QtSkillPlannerWidget() {
	delete allSkillsModel_;
}

void QtSkillPlannerWidget::setCharacter(Character::ref character) {
	allSkillsModel_->setRoot(dataController_->getSkillTree()->mergeWithCharacterSkills(character));
}

}

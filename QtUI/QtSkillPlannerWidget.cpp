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
	QTreeView* allSkillsWidget = new QTreeView(this);
	mainLayout->addWidget(allSkillsWidget);
	allSkillsModel_ = new QtSkillModel();
	QtSkillDelegate* allSkillsDelegate = new QtSkillDelegate(this);
	allSkillsWidget->setItemDelegate(allSkillsDelegate);
	allSkillsWidget->setModel(allSkillsModel_);
	allSkillsWidget->setUniformRowHeights(false);
	allSkillsWidget->setHeaderHidden(true);
	QTreeView* planWidget = new QTreeView(this);
	planModel_ = new QtSkillModel();
	QtSkillDelegate* planDelegate = new QtSkillDelegate(this);
	planWidget->setItemDelegate(planDelegate);
	planWidget->setModel(planModel_);
	planWidget->setUniformRowHeights(false);
	planWidget->setHeaderHidden(true);
	mainLayout->addWidget(planWidget);
	allSkillsWidget->setDragEnabled(true);
	planWidget->setAcceptDrops(true);
	planWidget->setDropIndicatorShown(true);
}

QtSkillPlannerWidget::~QtSkillPlannerWidget() {
	delete planModel_;
	delete allSkillsModel_;
}

void QtSkillPlannerWidget::setCharacter(Character::ref character) {
	allSkillsModel_->setRoot(dataController_->getSkillTree()->mergeWithCharacterSkills(character));
	planModel_->setRoot(character->getSkillPlanRoot());
}

}

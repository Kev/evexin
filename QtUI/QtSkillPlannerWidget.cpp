/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtSkillPlannerWidget.h>

#include <QBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeView>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>
#include <Eve-Xin/Controllers/SkillPlanList.h>
#include <Eve-Xin/Controllers/SkillTree.h>

#include <Eve-Xin/QtUI/QtSkillModel.h>
#include <Eve-Xin/QtUI/QtSkillDelegate.h>
#include <Eve-Xin/QtUI/QtTreeView.h>

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
	allSkillsWidget->setDragEnabled(true);

	planWidget_ = new QtTreeView(this);
	planModel_ = new QtSkillModel();
	QtSkillDelegate* planDelegate = new QtSkillDelegate(this);
	planWidget_->setItemDelegate(planDelegate);
	planWidget_->setModel(planModel_);
	planWidget_->setUniformRowHeights(false);
	planWidget_->setHeaderHidden(true);
	planWidget_->setDragEnabled(true);
	planWidget_->setAcceptDrops(true);
	allSkillsWidget->setDragEnabled(true);
	planWidget_->setAcceptDrops(true);
	planWidget_->setDropIndicatorShown(true);
	
	createPlanButton_ = new QPushButton("+", this);
	deletePlanButton_ = new QPushButton("-", this);
	QBoxLayout* buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	buttonLayout->addWidget(createPlanButton_);
	buttonLayout->addWidget(deletePlanButton_);

	QBoxLayout* planLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	planLayout->addWidget(planWidget_);
	planLayout->addLayout(buttonLayout);
	mainLayout->addLayout(planLayout);

	connect(createPlanButton_, SIGNAL(clicked()), this, SLOT(handleCreatePlanClicked()));
	connect(deletePlanButton_, SIGNAL(clicked()), this, SLOT(handleDeletePlanClicked()));
}

QtSkillPlannerWidget::~QtSkillPlannerWidget() {
	delete planModel_;
	delete allSkillsModel_;
}

void QtSkillPlannerWidget::setCharacter(Character::ref character) {
	character_ = character;
	allSkillsModel_->setRoot(dataController_->getSkillTree()->mergeWithCharacterSkills(character));
	planModel_->setRoot(character->getSkillPlanRoot());
	planModel_->setCharacter(character);
}

void QtSkillPlannerWidget::handleCreatePlanClicked() {
	if (!character_) {
		return;
	}
	bool ok;
	QString name = QInputDialog::getText(this, tr("Create Skill Plan"), tr("Skill plan name:"), QLineEdit::Normal, "", &ok);
	if (ok && !name.isEmpty()) {
		character_->getSkillPlanRoot()->createPlan(Q2PSTRING(name));
	}
}

void QtSkillPlannerWidget::handleDeletePlanClicked() {
	QModelIndexList indices = planWidget_->selectedIndexes();
	if (indices.size() > 0) {
		SkillItem::ref item = planModel_->getItem(indices[0]);
		SkillPlan::ref plan = boost::dynamic_pointer_cast<SkillPlan>(item);
		SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(item);
		if (plan) {
			QMessageBox messageBox;
			messageBox.setWindowTitle("Delete plan");
			messageBox.setText("Are you sure you want to delete the skill plan?");
			messageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
			messageBox.setDefaultButton(QMessageBox::Yes);
			if (messageBox.exec() == QMessageBox::Yes) {
				character_->getSkillPlanRoot()->deletePlan(plan);
			}
		}
		else if (level) {
			QMessageBox messageBox;
			messageBox.setWindowTitle("Remove skill from plan");
			messageBox.setText("Are you sure you want to the skill from the plan?");
			messageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
			messageBox.setDefaultButton(QMessageBox::Yes);
			if (messageBox.exec() == QMessageBox::Yes) {
				SkillPlan::ref parent = boost::dynamic_pointer_cast<SkillPlan>(level->getParent());
				parent->removeSkill(level->getSkill()->getID(), level->getLevel());
			}
		}
	}

}


}

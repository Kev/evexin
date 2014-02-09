/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtSkillPlannerWidget.h>

#include <boost/bind.hpp>

#include <QBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeView>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>
#include <Eve-Xin/Controllers/SkillPlanList.h>
#include <Eve-Xin/Controllers/SkillTime.h>
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
	undoButton_ = new QPushButton("Undo", this);
	suggestButton_ = new QPushButton("Suggest Remap", this);
	QBoxLayout* buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	buttonLayout->addWidget(createPlanButton_);
	buttonLayout->addWidget(deletePlanButton_);
	buttonLayout->addWidget(undoButton_);
	buttonLayout->addWidget(suggestButton_);

	QBoxLayout* planLayout = new QBoxLayout(QBoxLayout::TopToBottom);
	planLayout->addWidget(planWidget_);
	planLayout->addLayout(buttonLayout);
	mainLayout->addLayout(planLayout);

	connect(createPlanButton_, SIGNAL(clicked()), this, SLOT(handleCreatePlanClicked()));
	connect(deletePlanButton_, SIGNAL(clicked()), this, SLOT(handleDeletePlanClicked()));
	connect(undoButton_, SIGNAL(clicked()), this, SLOT(handleUndoClicked()));
	connect(suggestButton_, SIGNAL(clicked()), this, SLOT(handleSuggestClicked()));
	dataController_->onSkillTreeChanged.connect(boost::bind(&QtSkillPlannerWidget::handleSkillTreeChanged, this));
}

QtSkillPlannerWidget::~QtSkillPlannerWidget() {
	delete planModel_;
	delete allSkillsModel_;
}

void QtSkillPlannerWidget::handleSkillTreeChanged() {
	setCharacter(character_);
}

void QtSkillPlannerWidget::setCharacter(Character::ref character) {
	character_ = character;
	allSkillsModel_->setRoot(dataController_->getSkillTree()->mergeWithCharacterSkills(character));
	planModel_->setRoot(character ? character->getSkillPlanRoot() : SkillPlanList::ref());
	planModel_->setCharacter(character);
}

void QtSkillPlannerWidget::handleUndoClicked() {
	if (!character_) {
		return;
	}
	character_->getSkillPlanRoot()->undo();
	planModel_->setRoot(character_->getSkillPlanRoot()); // Cause it to reset the model. Ugly and needs fixing
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
			character_->getSkillPlanRoot()->deletePlan(plan);
		}
		else if (level) {
			SkillPlan::ref parent = boost::dynamic_pointer_cast<SkillPlan>(level->getParent());
			planModel_->removeSkill(parent, level->getSkill()->getID(), level->getLevel());
		}
	}

}

void QtSkillPlannerWidget::handleSuggestClicked() {
	QModelIndexList indices = planWidget_->selectedIndexes();
	SkillPlan::ref plan;
	if (indices.size() > 0) {
		SkillItem::ref item = planModel_->getItem(indices[0]);
		plan = boost::dynamic_pointer_cast<SkillPlan>(item);
	}
	if (plan) {
		std::pair<float, std::map<SkillAttribute::Attribute, int> > suggestions = SkillTime::suggestAttributes(plan);
		std::vector<SkillAttribute::Attribute> allAttributes = SkillAttribute::allAttributes();
		QString attributeString;
		foreach (auto attribute, allAttributes) {
			if (!attributeString.isEmpty()) {
				attributeString += ", ";
			}
			attributeString += QString("%1 %2").arg(P2QSTRING(SkillAttribute::toString(attribute))).arg(suggestions.second[attribute]);
		}
		QString text = QString("The suggested attributes for this plan, giving a time of %1 (excluding implants), is:  %2").arg(QtSkillDelegate::trainingTimeToString(suggestions.first)).arg(attributeString);
		QMessageBox box;
		box.setText(text);
		box.exec();
	}
}

}






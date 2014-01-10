/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtSkillModel.h>

#include <boost/bind.hpp>

#include <QColor>
#include <QIcon>
#include <QMimeData>
#include <qdebug.h>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Skill.h>
#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillLevel.h>
#include <Eve-Xin/Controllers/SkillPlan.h>

namespace EveXin {

QtSkillModel::QtSkillModel() {
}

QtSkillModel::~QtSkillModel() {
}

void QtSkillModel::setRoot(boost::shared_ptr<SkillItem> root) {
	beginResetModel();
	root_ = root;
	endResetModel();
}

Qt::DropActions QtSkillModel::supportedDropActions() const {
	return Qt::CopyAction;
}

Qt::ItemFlags QtSkillModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	if (index.model() != this) {
		// We can't get the pointer out, so just say it can be dropped
		flags |= Qt::ItemIsDropEnabled;
	}
	else {
		SkillItem::ref item = getItem(index);
		if (item && item->getSkill()) {
			flags |= Qt::ItemIsDragEnabled;
			flags |= Qt::ItemIsDropEnabled;	
		}
	}
	return flags;
}

int QtSkillModel::columnCount(const QModelIndex& /*parent*/) const {
	return 1;
}

boost::shared_ptr<SkillItem> QtSkillModel::getItem(const QModelIndex& index) const {
	boost::shared_ptr<SkillItem> result;
	SkillItem* internal = NULL;
	if (index.isValid()) {
		internal = static_cast<SkillItem*>(index.internalPointer());
	}
	if (internal) {
		result = internal->shared_from_this(); // now get the shared_ptr. Pointers to shared_ptrs, yuck.
	}
	return result;
}

QVariant QtSkillModel::getLevelData(SkillLevel::ref level, int role) const {
	switch (role) {
		case Qt::DisplayRole: {
			std::string name = level->getName();
			return P2QSTRING(name);
		}
		case SkillLevelRole: return level->getLevel();
		default: return getItemData(level, role);
	}
}

QVariant QtSkillModel::getItemData(SkillItem::ref item, int role) const {
	Skill::ref skill = item->getSkill();
	switch (role) {
		case Qt::DisplayRole: return P2QSTRING(item->getName());
		//case Qt::TextColorRole: return "not implemented :/";
		//case Qt::BackgroundColorRole: return "not implemented :/";
		case Qt::ToolTipRole: {
			std::string tooltip = skill ? skill->getDescription() /*+ "\n" + skill->getID()*/ : "";
			return QVariant(P2QSTRING(tooltip));
		}
		case IsSkillRole: return !!skill;
		case SkillMultiplierRole: return QVariant(skill->getRank());
		default: return QVariant();
	}
}

QVariant QtSkillModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid()) return QVariant();
	boost::shared_ptr<SkillItem> item = getItem(index);
	if (!item) return QVariant();

	SkillLevel::ref skillLevel = boost::dynamic_pointer_cast<SkillLevel>(item);
	return skillLevel ? getLevelData(skillLevel, role) : getItemData(item, role);
	
}

QModelIndex QtSkillModel::index(int row, int column, const QModelIndex& parent) const {
	if (!root_) {
		return QModelIndex();
	}
	boost::shared_ptr<SkillItem> parentItem;
	if (!parent.isValid()) {
		//top level
		parentItem = root_;
	} else {
		parentItem = getItem(parent);
		if (!parentItem) return QModelIndex();
	}
	return static_cast<size_t>(row) < parentItem->getChildren().size() ? createIndex(row, column, parentItem->getChildren()[row].get()) : QModelIndex();
}

QModelIndex QtSkillModel::index(boost::shared_ptr<SkillItem> item) const {
	if (!item) {
		return QModelIndex();
	}
	boost::shared_ptr<SkillItem> parent = item->getParent();
	/* Recursive check that it's ok to create such an item.*/
	if (!parent || !root_ || (parent != root_ && !index(parent).isValid())) {
		return QModelIndex();
	}
	for (size_t i = 0; i < parent->getChildren().size(); i++) {
		if (parent->getChildren()[i] == item) {
			return createIndex(i, 0, item.get());
		}
	}
	return QModelIndex();
}

QModelIndex QtSkillModel::parent(const QModelIndex& child) const {
	if (!root_ || !child.isValid()) {
		return QModelIndex();
	}
	
	boost::shared_ptr<SkillItem> parent = getItem(child)->getParent();
	return (parent != root_) ? index(parent) : QModelIndex();
}

int QtSkillModel::rowCount(const QModelIndex& parent) const {
	if (!root_) return 0;
	SkillItem::ref item = parent.isValid() ? getItem(parent) : root_;
	if (item == filtered_) {
		//We're in the middle of resetting this item's children, say it has none.
		return 0;
	}
	assert(item.get() != filtered_.get());
	Q_ASSERT(item);
	int count = item->getChildren().size();
	// qDebug() << "Returning " << count << " rows";
	return count;
}

QMimeData* QtSkillModel::mimeData(const QModelIndexList& indexes) const {
	QMimeData* data = QAbstractItemModel::mimeData(indexes);

	if (indexes.size() < 1) {
		return data;
	}

	SkillItem::ref item = getItem(indexes[0]);
	SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(item);

	if (!item || !item->getSkill()) {
		return data;
	}

	QByteArray getItemData;
	QDataStream dataStream(&getItemData, QIODevice::WriteOnly);

	dataStream << P2QSTRING(item->getSkill()->getID());
	int draggedLevel = -1;
	if (level) {
		if (boost::dynamic_pointer_cast<SkillPlan>(level->getParent())) {
			// If we're dragging inside a plan list, we just want to move the skill the plan
			// Otherwise leave it as 'next level'
			draggedLevel = level->getLevel();
		}
	}
	dataStream << draggedLevel;
	data->setData("application/vnd.evexin.skilllevel", getItemData);
	return data;
}

bool QtSkillModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {
	if (action == Qt::IgnoreAction || !data->hasFormat("application/vnd.evexin.skilllevel") || !root_) {
		qDebug() << "Reject drop";
		return false;
	}
	QModelIndex adjustedParent(parent);
	SkillItem::ref parentItem(getItem(parent));
	SkillPlan::ref plan = boost::dynamic_pointer_cast<SkillPlan>(parentItem);
	if (parentItem && !plan) {
		//We're being dropped in an item, but it's not a plan. Maybe it's a child of a plan
		SkillItem::ref grandParent = parentItem->getParent();
		plan = boost::dynamic_pointer_cast<SkillPlan>(grandParent);
		row =  parent.row();
	}
	if (!plan) {
		// If they drop it off the end of the list, use the last plan
		SkillItem::ref lastItem = root_->getChildren().empty() ? SkillItem::ref() : root_->getChildren().back();
		plan = boost::dynamic_pointer_cast<SkillPlan>(lastItem);
		if (!plan) {
			//OK, we're not a skill plan tree
			qDebug() << "Received a drop, but we're not a plan";
			return false;
		}
	}
	adjustedParent = index(plan);
	QByteArray encodedData = data->data("application/vnd.evexin.skilllevel");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QString id;
	stream >> id;
	int level;
	stream >> level;
	std::string skillID = Q2PSTRING(id);
	size_t rowT = row >= 0 ? static_cast<size_t>(row) : plan->getChildren().size();
	beginRemoveRows(adjustedParent, 0, plan->getChildren().size() - 1);
	filtered_ = plan;
	endRemoveRows();
	plan->addSkill(skillID, level, rowT);
	beginInsertRows(adjustedParent, 0, plan->getChildren().size() - 1);
	filtered_.reset();
	endInsertRows();
	return true;
}

}


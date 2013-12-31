/*
 * Copyright (c) 2013 Kevin Smith
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

Qt::ItemFlags QtSkillModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	// if (dynamic_cast<GroupRosterItem*>(getItem(index)) == NULL) {
	// 	flags |= Qt::ItemIsDragEnabled;
	// }
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
		case Qt::ToolTipRole: return skill ? QVariant(P2QSTRING(skill->getDescription())) : QVariant();
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
	Q_ASSERT(item);
	int count = item->getChildren().size();
	//qDebug() << "Returning " << count << " rows";
	return count;
}

// QMimeData* QtSkillModel::mimeData(const QModelIndexList& indexes) const {
// 	QMimeData* data = QAbstractItemModel::mimeData(indexes);

// 	ContactRosterItem *item = dynamic_cast<ContactRosterItem*>(getItem(indexes.first()));
// 	if (item == NULL) {
// 		return data;
// 	}

// 	QByteArray getItemData;
// 	QDataStream dataStream(&getItemData, QIODevice::WriteOnly);

// 	// jid, chatName, activity, statusType, avatarPath
// 	dataStream << P2QSTRING(item->getJID().toString());
// 	dataStream << P2QSTRING(item->getDisplayName());
// 	dataStream << P2QSTRING(item->getStatusText());
// 	dataStream << item->getSimplifiedStatusShow();
// 	dataStream << P2QSTRING(item->getAvatarPath().string());
// 	data->setData("application/vnd.swift.contact-jid", getItemData);
// 	return data;
// }

}

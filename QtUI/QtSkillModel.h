/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QAbstractItemModel>
#include <QList>


namespace EveXin {

	class SkillItem;
	class SkillLevel;
	class Character;

	class QtSkillModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			enum SkillRoles {
				IsSkillRole = Qt::UserRole,
				SkillMultiplierRole = Qt::UserRole + 1,
				SkillLevelRole = Qt::UserRole + 2,
				SkillTrainingTimeRole = Qt::UserRole + 3,
				ChildCountRole = Qt::UserRole + 4,
			};
			QtSkillModel();
			~QtSkillModel();
			void setRoot(boost::shared_ptr<SkillItem> root);
			void setCharacter(boost::shared_ptr<Character> character);
			Qt::ItemFlags flags(const QModelIndex& index) const;
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
			QModelIndex index(boost::shared_ptr<SkillItem> item) const;
			QModelIndex parent(const QModelIndex& index) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			Qt::DropActions supportedDropActions() const;
			QMimeData* mimeData(const QModelIndexList& indexes) const;
			bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
			boost::shared_ptr<SkillItem> getItem(const QModelIndex& index) const;
		// signals:
		// 	void itemExpanded(const QModelIndex& item, bool expanded);
		private:
			QVariant getItemData(boost::shared_ptr<SkillItem> item, int role) const;
			QVariant getLevelData(boost::shared_ptr<SkillLevel> level, int role) const;
			// void handleDataChanged(RosterItem* item);
			// void handleChildrenChanged(GroupRosterItem* item);
			// QColor intToColor(int color) const;
			// QColor getTextColor(RosterItem* item) const;
			// QColor getBackgroundColor(RosterItem* item) const;
			// QString getToolTip(RosterItem* item) const;
			// QString getAvatar(RosterItem* item) const;
			// QString getStatusText(RosterItem* item) const;
			// QIcon getPresenceIcon(RosterItem* item) const;
			// int getChildCount(RosterItem* item) const;
			// bool getIsIdle(RosterItem* item) const;
			// void reLayout();
		private:
			boost::shared_ptr<SkillItem> root_;
			boost::shared_ptr<SkillItem> filtered_;
			boost::shared_ptr<Character> character_;
	};
}

/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QAbstractItemModel>
#include <QList>


namespace EveXin {
	// enum RosterRoles {
	// 	StatusTextRole = Qt::UserRole,
	// };

	class SkillItem;
	class SkillLevel;

	class QtSkillModel : public QAbstractItemModel {
		Q_OBJECT
		public:
			QtSkillModel();
			~QtSkillModel();
			void setRoot(boost::shared_ptr<SkillItem> root);
			Qt::ItemFlags flags(const QModelIndex& index) const;
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
			QModelIndex index(boost::shared_ptr<SkillItem> item) const;
			QModelIndex parent(const QModelIndex& index) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			//QMimeData* mimeData(const QModelIndexList& indexes) const;

		// signals:
		// 	void itemExpanded(const QModelIndex& item, bool expanded);
		private:
			boost::shared_ptr<SkillItem> getItem(const QModelIndex& index) const;
			QVariant itemData(boost::shared_ptr<SkillItem> item, int role) const;
			QVariant levelData(boost::shared_ptr<SkillLevel> level, int role) const;
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
	};
}

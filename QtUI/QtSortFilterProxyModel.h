/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QSortFilterProxyModel>

namespace EveXin {
	class QtSortFilterProxyModel : public QSortFilterProxyModel {
		Q_OBJECT
		public:
			QtSortFilterProxyModel(QObject* parent = NULL);
			virtual ~QtSortFilterProxyModel();
		protected:
			virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
	};
}

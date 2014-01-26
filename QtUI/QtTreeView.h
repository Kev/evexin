/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QTreeView>

namespace EveXin {
	class QtTreeView : public QTreeView {
		public:
			QtTreeView(QWidget* parent) : QTreeView(parent) {
				
			}

			virtual QModelIndexList selectedIndexes() const {
				return QTreeView::selectedIndexes();
			}
	};
}

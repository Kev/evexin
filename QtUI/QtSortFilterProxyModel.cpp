/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtSortFilterProxyModel.h>

namespace EveXin {

QtSortFilterProxyModel::QtSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
		
}

QtSortFilterProxyModel::~QtSortFilterProxyModel() {
	
}

bool QtSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
	if (sourceParent.isValid()) {
		return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
	}
	QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
	bool visibleChild = false;
	for (int i = 0; i < sourceModel()->rowCount(index); i++) {
		visibleChild |= filterAcceptsRow(i, index);
	}
	return visibleChild;
}

}








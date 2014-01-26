/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QStyledItemDelegate>

namespace EveXin {
	class CharacterListDelegate : public QStyledItemDelegate {
		Q_OBJECT
		public:
			CharacterListDelegate(QObject* parent);

			virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		private:
			static const int horizontalMargin;
			static const int verticalMargin;
			static const int farLeftMargin;
			static const int avatarSize;
			QFont nameFont_;
			QFont detailFont_;
	};
}

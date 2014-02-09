/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QFont>
#include <QSize>
#include <QString>
#include <QStyledItemDelegate>

namespace EveXin {
	class QtSkillDelegate : public QStyledItemDelegate {
		public:
			QtSkillDelegate(QObject* parent);
			QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
			static QString trainingTimeToString(float minutes);
		private:
			QFont nameFont_;
			QFont infoFont_;
	};
}

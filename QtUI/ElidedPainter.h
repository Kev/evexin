/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QPainter>
#include <QRect>
#include <QString>

namespace EveXin {
	class ElidedPainter {
		public:
			static void drawElidedText(QPainter* painter, const QRect& region, const QString& text, int flags = Qt::AlignTop) {
				QString adjustedText(painter->fontMetrics().elidedText(text, Qt::ElideRight, region.width(), Qt::TextShowMnemonic));
				painter->setClipRect(region);
				painter->drawText(region, flags, adjustedText.simplified());
				painter->setClipping(false);
			}
	};
}

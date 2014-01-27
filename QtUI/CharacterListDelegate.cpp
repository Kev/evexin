/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/CharacterListDelegate.h>

#include <QApplication>
#include <QFontMetrics>
#include <QSize>

#include <Eve-Xin/QtUI/CharacterListModel.h>
#include <Eve-Xin/QtUI/ElidedPainter.h>

namespace EveXin {

const int CharacterListDelegate::horizontalMargin = 2;
const int CharacterListDelegate::verticalMargin = 2;
const int CharacterListDelegate::farLeftMargin = 2;
const int CharacterListDelegate::avatarSize = 32;

CharacterListDelegate::CharacterListDelegate(QObject* parent) : QStyledItemDelegate(parent), nameFont_(QApplication::font()), detailFont_(QApplication::font()) {
	int detailFontSizeDrop = nameFont_.pointSize() >= 10 ? 2 : 0;
	detailFont_.setStyle(QFont::StyleItalic);
	detailFont_.setPointSize(nameFont_.pointSize() - detailFontSizeDrop);
}

QSize CharacterListDelegate::sizeHint(const QStyleOptionViewItem& /*option*/, const QModelIndex& /*index*/) const {
	int heightByAvatar = avatarSize + verticalMargin * 2;
	QFontMetrics nameMetrics(nameFont_);
	QFontMetrics detailMetrics(detailFont_);
	int sizeByText = 2 * verticalMargin + nameMetrics.height() + detailMetrics.height();
	return QSize(150, sizeByText > heightByAvatar ? sizeByText : heightByAvatar);
}

void CharacterListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	painter->save();
	QRect fullRegion(option.rect);
	if (option.state & QStyle::State_Selected) {
		painter->fillRect(fullRegion, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());
	} else {
		painter->setPen(option.palette.text().color());
	}

	QRect avatarRegion(QPoint(farLeftMargin, fullRegion.top() + verticalMargin), QSize(avatarSize, avatarSize));

	//NOTE: This is pretty expensive for a paint(), I may need to change this.
	QByteArray avatarData = index.data(CharacterListModel::AvatarRole).toByteArray();
	QImage avatarImage;
	avatarImage.loadFromData(avatarData, "JPG");
	QPixmap avatarPixmap = QPixmap::fromImage(avatarImage);
	painter->drawPixmap(avatarRegion.topLeft() + QPoint(((avatarRegion.width() - avatarPixmap.width()) / 2), (avatarRegion.height() - avatarPixmap.height()) / 2), avatarPixmap);

	QFontMetrics nameMetrics(nameFont_);
	painter->setFont(nameFont_);
	int extraFontWidth = nameMetrics.width("H");
	int leftOffset = avatarRegion.right() + horizontalMargin * 2 + extraFontWidth / 2;
	QRect textRegion(fullRegion.adjusted(leftOffset, 0, 0/*-leftOffset*/, 0));

	int nameHeight = nameMetrics.height() + verticalMargin;
	QRect nameRegion(textRegion.adjusted(0, verticalMargin, 0, 0));

	QString name = index.data(Qt::DisplayRole).toString();
	ElidedPainter::drawElidedText(painter, nameRegion, name);

	painter->setFont(detailFont_);
	painter->setPen(QPen(QColor(160,160,160)));

	QString detailText = index.data(CharacterListModel::DetailTextRole).toString();
	QRect detailTextRegion(textRegion.adjusted(0, nameHeight, 0, 0));
	ElidedPainter::drawElidedText(painter, detailTextRegion, detailText);

	painter->restore();
}

}

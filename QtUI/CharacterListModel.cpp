/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/CharacterListModel.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace EveXin {

CharacterListModel::CharacterListModel(QObject* parent) : QAbstractListModel(parent) {

}

int CharacterListModel::rowCount(const QModelIndex& parent) const {
	if (parent.isValid()) {
		return 0;
	}
	return static_cast<int>(characters_.size());
}

QByteArray CharacterListModel::getAvatarData(Character::ref character) const {
	Swift::ByteArray avatarData = character ? character->getAvatar(32) : Swift::ByteArray();
	//NOTE: reinterpret_cast alert. Don't do this at home.
	return QByteArray(reinterpret_cast<char *>(&avatarData[0]), avatarData.size());
}

QVariant CharacterListModel::data(const QModelIndex& index, int role) const {
	if (!index.isValid()) {
		return QVariant();
	}
	Character::ref character = characters_[index.row()];
	switch (role) {
		case Qt::DisplayRole: return QVariant(P2QSTRING(character->getName()));
		case AvatarRole:return getAvatarData(character);
		default: return QVariant();
	}
}

void CharacterListModel::setCharacters(const std::vector<Character::ref>& characters) {
	beginResetModel();
	characters_ = characters;
	endResetModel();
}

}

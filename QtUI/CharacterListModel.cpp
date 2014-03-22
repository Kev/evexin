/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/CharacterListModel.h>

#include <boost/bind.hpp>

#include <QTimer>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/SkillTime.h>

namespace EveXin {

CharacterListModel::CharacterListModel(QObject* parent) : QAbstractListModel(parent) {
	QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTick()));
    timer->start(1000);
}

void CharacterListModel::handleTick() {
	if (characters_.empty()) {
		return;
	}
	//Recalculate the times remaining
	QModelIndex topLeft = index(0);
	QModelIndex bottomRight = index(static_cast<int>(characters_.size()));
	dataChanged(topLeft, bottomRight);
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
		case AvatarRole: return getAvatarData(character);
		case SkillTimeRole: return !!character->getTrainingQueue() ? SkillTime::minutesToTrainAll(character, character->getTrainingQueue()) : QVariant();
		case SkillTimeHighlightRole: return !!character->getTrainingQueue() ? QVariant(SkillTime::minutesToTrainAll(character, character->getTrainingQueue()) < 24 * 60) : QVariant();
		default: return QVariant();
	}
}

void CharacterListModel::setCharacters(const std::vector<Character::ref>& characters) {
	beginResetModel();
	foreach (Character::ref character, characters_) {
		character->onDataChanged.disconnect(boost::bind(&CharacterListModel::handleCharacterDataChanged, this, character));
	}
	characters_ = characters;
	foreach (Character::ref character, characters_) {
		character->onDataChanged.connect(boost::bind(&CharacterListModel::handleCharacterDataChanged, this, character));
	}
	endResetModel();
}

void CharacterListModel::handleCharacterDataChanged(Character::ref character) {
	QModelIndex characterIndex;
	for (size_t i = 0; i < characters_.size(); i++) {
		if (character == characters_[i]) {
			characterIndex = index(static_cast<int>(i));
		}
	}
	if (!characterIndex.isValid()) {
		return;
	}
	dataChanged(characterIndex, characterIndex);
}

}

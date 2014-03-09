/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>

#include <QAbstractListModel>

#include <Eve-Xin/Controllers/Character.h>

namespace EveXin {
	class CharacterListModel : public QAbstractListModel {
		Q_OBJECT
		public:
			enum CharacterRoles {
				AvatarRole = Qt::UserRole,
				SkillTimeRole = Qt::UserRole + 1,
				SkillTimeHighlightRole = Qt::UserRole + 2
			};
			CharacterListModel(QObject* parent);
			virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
			virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
			void setCharacters(const std::vector<Character::ref>& characters);
		private slots:
			void handleTick();
		private:
			QByteArray getAvatarData(Character::ref character) const;
		private:
			std::vector<Character::ref> characters_;
	};
}

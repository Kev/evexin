/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QListView>

#include <Swiften/Base/boost_bsignals.h>

#include <Eve-Xin/Controllers/Character.h>

namespace EveXin {
	class CharacterListModel;
	class QtCharacterList : public QListView {
		Q_OBJECT
		public:
			QtCharacterList(QWidget* parent);
			~QtCharacterList();

			Character::ref getCurrentCharacter();
			void setCharacters(const std::vector<Character::ref>& characters);

		public:
			boost::signal<void(Character::ref)> onCharacterSelected;

		private slots:
			void handleItemActivated(const QModelIndex& index);
		private:
			std::vector<Character::ref> characters_;
			CharacterListModel* model_;
			size_t selectedRow_;
	};
}

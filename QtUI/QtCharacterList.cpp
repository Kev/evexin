/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtCharacterList.h>

#include <limits>

#include <QItemSelectionModel>

#include <Eve-Xin/QtUI/CharacterListModel.h>
#include <Eve-Xin/QtUI/CharacterListDelegate.h>

namespace EveXin {

QtCharacterList::QtCharacterList(QWidget* parent) : QListView(parent), selectedRow_(std::numeric_limits<size_t>::max()) {
	model_ = new CharacterListModel(this);
	setModel(model_);
	delegate_ = new CharacterListDelegate(this);
	setItemDelegate(delegate_);
	connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
	connect(this, SIGNAL(activated(const QModelIndex&)), this, SLOT(handleItemActivated(const QModelIndex&)));
}

QtCharacterList::~QtCharacterList() {
	
}

Character::ref QtCharacterList::getCurrentCharacter() {
	//NOTE: If we start using proxies, this will likely break
	return selectedRow_ < characters_.size() ? characters_[selectedRow_] : Character::ref();
}

void QtCharacterList::setCharacters(const std::vector<Character::ref>& characters) {
	characters_ = characters;
	model_->setCharacters(characters);
	if (selectionModel()->selectedIndexes().empty()) {
		QModelIndex firstIndex = model_->index(0);
		selectionModel()->select(firstIndex, QItemSelectionModel::Select);
		handleItemActivated(firstIndex);
	}
}

void QtCharacterList::handleItemActivated(const QModelIndex& index) {
	selectedRow_ = static_cast<size_t>(index.row());
	onCharacterSelected(getCurrentCharacter());
}

}

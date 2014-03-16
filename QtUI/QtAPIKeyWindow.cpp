/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtAPIKeyWindow.h>

#include <string>
#include <vector>

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>

#include <Swiften/Base/foreach.h>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/DataController.h>

namespace EveXin {

QtAPIKeyWindow::QtAPIKeyWindow(boost::shared_ptr<DataController> dataController) : dataController_(dataController) {
	setAttribute(Qt::WA_QuitOnClose, false); // Don't keep the application alive because I'm visible
	setAttribute(Qt::WA_DeleteOnClose, false); // Keep me alive when I'm hidden

	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	keysList_ = new QListWidget(this);
	mainLayout->addWidget(keysList_);
	
	deleteButton_ = new QPushButton("Remove Key", this);
	deleteButton_->setEnabled(false);
	mainLayout->addWidget(deleteButton_);

	QBoxLayout* addKeyLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	mainLayout->addLayout(addKeyLayout);

	QLabel* label = new QLabel("API Key", this);
	addKeyLayout->addWidget(label);
	keyEdit_ = new QLineEdit(this);
	addKeyLayout->addWidget(keyEdit_);
	label = new QLabel("Verification Code", this);
	addKeyLayout->addWidget(label);
	verEdit_ = new QLineEdit(this);
	addKeyLayout->addWidget(verEdit_);
	addButton_ = new QPushButton("Add Key", this);
	addKeyLayout->addWidget(addButton_);

	connect(addButton_, SIGNAL(clicked()), this, SLOT(handleAddKey()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(handleDeleteKey()));
	connect(keysList_, SIGNAL(itemSelectionChanged()), this, SLOT(handleSelectionChanged()));
	populateKeys();
}

void QtAPIKeyWindow::handleAddKey() {
	dataController_->addAPIKey(Q2PSTRING(keyEdit_->text()), Q2PSTRING(verEdit_->text()));
	populateKeys();
}

void QtAPIKeyWindow::populateKeys() {
	std::vector<std::string> keys = dataController_->getAPIKeys();
	keysList_->clear();
	foreach (std::string key, keys) {
		std::vector<Character::ref> characters = dataController_->getCharactersForAPIKey(key);
		std::string charactersString;
		foreach (Character::ref character, characters) {
			charactersString += charactersString.empty() ? " - " : ", ";
			charactersString += character->getName();
		}
		if (charactersString.empty()) {
			charactersString += " - Characters Unknown";
		}
		std::string keyString = key + charactersString;
		keysList_->addItem(P2QSTRING(keyString));
	}
	handleSelectionChanged();
}

void QtAPIKeyWindow::handleSelectionChanged() {
	deleteButton_->setEnabled(!keysList_->selectedItems().isEmpty());
}

void QtAPIKeyWindow::handleDeleteKey() {
	if (keysList_->selectedItems().isEmpty()) {
		return;
	}
	QMessageBox messageBox;
	messageBox.setWindowTitle("Delete API Key");
	QString key = keysList_->selectedItems()[0]->text().split(" ")[0]; // To remove the annotation
	messageBox.setText(tr("Are you sure you want to delete API Key %1?").arg(key));
	messageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	messageBox.setDefaultButton(QMessageBox::Yes);
	if (messageBox.exec() == QMessageBox::Yes) {
		dataController_->deleteAPIKey(Q2PSTRING(key));
	}
	populateKeys();
}

}




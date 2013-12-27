/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtAPIKeyWindow.h>

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#include <Swift/QtUI/QtSwiftUtil.h>

#include <Eve-Xin/Controllers/DataController.h>

namespace EveXin {

QtAPIKeyWindow::QtAPIKeyWindow(boost::shared_ptr<DataController> dataController) : dataController_(dataController) {
	setAttribute(Qt::WA_QuitOnClose, false); // Don't keep the application alive because I'm visible
	setAttribute(Qt::WA_DeleteOnClose, false); // Keep me alive when I'm hidden

	QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
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
}

void QtAPIKeyWindow::handleAddKey() {
	dataController_->addAPIKey(Q2PSTRING(keyEdit_->text()), Q2PSTRING(verEdit_->text()));
}

}




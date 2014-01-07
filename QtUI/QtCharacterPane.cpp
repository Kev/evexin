/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtCharacterPane.h>

#include <boost/lexical_cast.hpp>

#include <QBoxLayout>
#include <QGridLayout>
#include <QImage>
#include <QLabel>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUtilities.h>

namespace EveXin {

QtCharacterPane::QtCharacterPane(QWidget* parent) : QWidget(parent) {
	QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
	avatarLabel_ = new QLabel(this);
	avatarLabel_->setMinimumSize(256, 256);
	layout->addWidget(avatarLabel_);
	nameLabel_ = new QLabel(this);
	//nameLabel_->setBold();
	layout->addWidget(nameLabel_);
	QGridLayout* infoLayout = new QGridLayout();
	layout->addLayout(infoLayout);
	QLabel* label = new QLabel("Corporation: ", this);
	corpLabel_ = new QLabel(this);
	infoLayout->addWidget(label, 0, 0);
	infoLayout->addWidget(corpLabel_, 0, 1);
	//allianceLabel_ = new QLabel(this);
	//infoLayout->addWidget(allianceLabel_, 1, 1);
	iskLabel_ = new QLabel(this);
	label = new QLabel("ISK: ", this);
	infoLayout->addWidget(label, 1, 0);
	infoLayout->addWidget(iskLabel_, 1, 1);
	skillPointsLabel_ = new QLabel(this);
	layout->addWidget(skillPointsLabel_);
	cloneLabel_ = new QLabel(this);
	layout->addWidget(cloneLabel_);
	setCharacter(Character::ref());
}

void QtCharacterPane::setCharacter(Character::ref character) {
	nameLabel_->setText(QString("<b>%1</b>").arg(QtUtilities::htmlEscape(character ? P2QSTRING(character->getName()) : "No Character Selected")));
	Swift::ByteArray avatarData = character ? character->get256Avatar() : Swift::ByteArray(); //dataController_->get256CharacterAvatar(id);
	QImage avatarImage;
	avatarImage.loadFromData(&avatarData[0], avatarData.size(), "JPG");
	avatarLabel_->setPixmap(QPixmap::fromImage(avatarImage));
	corpLabel_->setText(character ? P2QSTRING(character->getCorpName()) : "No Corp");
	//expiresLabel_->setText(P2QSTRING(character->getExpires()));
	iskLabel_->setText(character ? P2QSTRING(boost::lexical_cast<std::string>(character->getISK())) : "0.00");
}

}

/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtCharacterPane.h>

#include <boost/bind.hpp>
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
	int row = 2;
	std::vector<SkillAttribute::Attribute> allAttributes = SkillAttribute::allAttributes();
	foreach (SkillAttribute::Attribute attribute, allAttributes) {
		label = new QLabel(P2QSTRING(SkillAttribute::toString(attribute)) + ": ", this);
		infoLayout->addWidget(label, row, 0);
		QLabel* valueLabel = new QLabel(this);
		infoLayout->addWidget(valueLabel, row, 1);
		++row;
		attributeLabels_[attribute] = valueLabel;
	}
	skillPointsLabel_ = new QLabel(this);
	layout->addWidget(skillPointsLabel_);
	cloneLabel_ = new QLabel(this);
	layout->addWidget(cloneLabel_);
	
	setCharacter(Character::ref());
}

QtCharacterPane::~QtCharacterPane() {
	setCharacter(Character::ref());
}

void QtCharacterPane::setCharacter(Character::ref character) {
	bool needsBind = false;
	if (character_) {
		if (character_ != character) {
			character_->onDataChanged.disconnect(boost::bind(&QtCharacterPane::setCharacter, this, character_));
			needsBind = true;
		}
	}
	else {
		needsBind = true;
	}

	if (needsBind && character) {
		character->onDataChanged.connect(boost::bind(&QtCharacterPane::setCharacter, this, character));
	}
	character_ = character;

	nameLabel_->setText(QString("<b>%1</b>").arg(QtUtilities::htmlEscape(character ? P2QSTRING(character->getName()) : "No Character Selected")));
	Swift::ByteArray avatarData = character ? character->getAvatar(256) : Swift::ByteArray(); //dataController_->get256CharacterAvatar(id);
	QImage avatarImage;
	avatarImage.loadFromData(&avatarData[0], avatarData.size(), "JPG");
	avatarLabel_->setPixmap(QPixmap::fromImage(avatarImage));
	corpLabel_->setText(character ? P2QSTRING(character->getCorpName()) : "No Corp");
	//expiresLabel_->setText(P2QSTRING(character->getExpires()));
	float isk = character ? character->getISK() : 0;
	iskLabel_->setText(QString("%L1").arg(isk, 0, 'f', 2));
	std::vector<SkillAttribute::Attribute> allAttributes = SkillAttribute::allAttributes();
	foreach (SkillAttribute::Attribute attribute, allAttributes) {
		QLabel* label = attributeLabels_[attribute];
		QString value;
		if (character) {
			int bonus = character->getImplantValue(attribute);
			value = QString("%1 %2").arg(character->getAttribute(attribute)).arg(bonus ? QString("+%1"/*\n (%2)"*/).arg(bonus)/*.arg(P2QSTRING(character->getImplantName(attribute)))*/ : QString(""));
		}
		label->setText(value);
	}
}

}







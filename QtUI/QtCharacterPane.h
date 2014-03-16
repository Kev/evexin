/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/shared_ptr.hpp>

#include <QWidget>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/SkillAttribute.h>

class QLabel;


namespace EveXin {
	class QtCharacterPane : public QWidget {
		Q_OBJECT
		public:
			QtCharacterPane(QWidget* parent = NULL);
			~QtCharacterPane();
    		void setCharacter(Character::ref character);
		private:
			QLabel* avatarLabel_;
			QLabel* nameLabel_;
			QLabel* corpLabel_;
			//QLabel* allianceLabel_;
			QLabel* iskLabel_;
			QLabel* skillPointsLabel_;
			QLabel* cloneLabel_;
			std::map<SkillAttribute::Attribute, QLabel*> attributeLabels_;
			Character::ref character_;

	};
}

/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillLevel.h>

#include <boost/shared_ptr.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/Skill.h>

namespace EveXin {

SkillLevel::SkillLevel(boost::shared_ptr<SkillItem> parent, boost::shared_ptr<Skill> skill, int level) : SkillItem(parent, skill), level_(level) {
	
}

int SkillLevel::getLevel() const {
	return level_;
}

int SkillLevel::getSkillPoints() const {
	int multiplier = getSkill()->getRank();
	int points = 0;
	switch (level_) {
		case 1: points = 250; break;
		case 2: points = 1414; break;
		case 3: points = 8000; break;
		case 4: points = 45255; break;
		case 5: points = 256000; break;
	}
	points *= multiplier;
	return points;
}

int SkillLevel::getSkillPointsSinceLastLevel() const {
	int multiplier = getSkill()->getRank();
	int points = 0;
	switch (level_) {
		case 1: points = 250; break;
		case 2: points = 1414 - 250; break;
		case 3: points = 8000 - 1414; break;
		case 4: points = 45255 - 8000; break;
		case 5: points = 256000 - 45255; break;
	}
	points *= multiplier;
	return points;
}


}

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

SkillLevel::SkillLevel(boost::shared_ptr<SkillItem> parent, boost::shared_ptr<Skill> skill, int level, int pointsTrained) : SkillItem(parent, skill), level_(level), skillPoints_(pointsTrained) {
	
}

int SkillLevel::getLevel() const {
	return level_;
}

int SkillLevel::rawLevelPoints(int level) const {
	switch (level) {
		case 1: return 250;
		case 2: return 1414;
		case 3: return 8000;
		case 4: return 45255;
		case 5: return 256000;
		default: return 0;
	}
}

int SkillLevel::getSkillPointsForLevel() const {
	int multiplier = getSkill()->getRank();
	int points = 0;
	points = rawLevelPoints(level_);
	points *= multiplier;
	return points;
}

int SkillLevel::getSkillPointsTrained() const {
	return (skillPoints_ >= 0) ? skillPoints_ : rawLevelPoints(level_);
}

int SkillLevel::getRemainingSkillPoints() const {
	int multiplier = getSkill()->getRank();
	int points = getSkillPointsForLevel() - getSkillPointsTrained();
	points *= multiplier;
	std::cerr << points << " remaining for " << getSkill()->getName() << ", " << getSkillPointsTrained() << " already trained "<< std::endl;
	return points;
}

bool SkillLevel::isComplete() const {
	return getRemainingSkillPoints() == 0;
}

}

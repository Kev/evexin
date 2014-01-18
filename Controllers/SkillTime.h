/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {
	class SkillTime {
		public:
			static float minutesToTrain(Character::ref character, SkillLevel::ref skillLevel) {
				int pointsNeeded = skillLevel->getSkillPointsSinceLastLevel();
				//If already training, instead getSkillPointsNeeded() - those in the character;
				int primaryAttribute = character->getAttribute(skillLevel->getSkill()->getPrimaryAttribute());
				int secondaryAttribute = character->getAttribute(skillLevel->getSkill()->getSecondaryAttribute());
				float minutes = pointsNeeded / (primaryAttribute + secondaryAttribute / 2);
				return minutes;
			}

			static boost::posix_time::ptime timeToTrain(Character::ref character, SkillLevel::ref skillLevel) {
				boost::posix_time::ptime startTime = boost::posix_time::second_clock::local_time();
				return startTime + boost::posix_time::minutes(minutesToTrain(character, skillLevel));
			}

			static float minutesToTrainAll(Character::ref character, SkillItem::ref item) {
				SkillLevel::ref level;
				if ((level = boost::dynamic_pointer_cast<SkillLevel>(item))) {
					return minutesToTrain(character, level);
				}
				float total = 0;
				std::vector<SkillItem::ref> children = item->getChildren();
				foreach (SkillItem::ref child, children) {
					total += minutesToTrainAll(character, child);
				}
				return total;
			}
	};
};









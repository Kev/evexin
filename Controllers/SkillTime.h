/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {
	class SkillTime {
		public:
			static std::string timeToTrain(Character::ref character, SkillLevel::ref skillLevel) {
				int pointsNeeded = skillLevel->getSkillPointsSinceLastLevel();
				int primaryAttribute = 20; //FIXME
				int secondaryAttribute = 20; //FIXME
				float minutes = pointsNeeded / (primaryAttribute + secondaryAttribute / 2);
			}
	};
};









/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>

namespace EveXin {
	class Skill;
	class SkillLevel : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillLevel> ref;
			SkillLevel(boost::weak_ptr<Skill> skill, int level) : skill_(skill), level_(level) {}

			int getLevel() {return level_;}
			boost::shared_ptr<Skill> getSkill() {return skill_;}
		private:
			boost::shared_ptr<Skill> skill_;
			int level_;
	};
}

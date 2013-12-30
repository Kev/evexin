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

}

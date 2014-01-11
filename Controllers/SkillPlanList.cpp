/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlanList.h>

namespace EveXin {
	
SkillPlanList::SkillPlanList(const std::string id, const std::string& name, boost::shared_ptr<SkillTree> allSkills) : SkillItem(SkillItem::ref(), id, name), allSkills_(allSkills){
	
}


SkillPlanList::~SkillPlanList() {
	
}

}

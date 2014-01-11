/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlanList.h>

#include <boost/lexical_cast.hpp>

namespace EveXin {
	
SkillPlanList::SkillPlanList(const std::string id, const std::string& name, boost::shared_ptr<SkillTree> allSkills) : SkillItem(SkillItem::ref(), id, name), allSkills_(allSkills), nextID_(0) {
	
}


SkillPlanList::~SkillPlanList() {
	
}

SkillPlan::ref SkillPlanList::createPlan(const std::string& name) {
	SkillPlan::ref plan = boost::make_shared<SkillPlan>(shared_from_this(), getID() + "_plan_" + boost::lexical_cast<std::string>(nextID_++), name, allSkills_);
	addChild(plan);
	return plan;
}

void SkillPlanList::deletePlan(SkillPlan::ref plan) {
	children_.erase(plan->getID());
}

}

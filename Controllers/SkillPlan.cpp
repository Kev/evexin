/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlan.h>

namespace EveXin {

SkillPlan::SkillPlan(SkillItem::ref parent, const std::string& id, const std::string& name) : SkillItem(parent, id, name) {

}

SkillPlan::~SkillPlan() {

}

bool SkillPlan::addSkill(Skill::ref skill) {
	int level = 5;
	return addSkill(skill, level);
}

bool SkillPlan::addSkill(Skill::ref skill, int level) {
	SkillLevel::ref found = plannedSkills_[skill->getID()];
	//std::cerr << "Adding " << skill->getName() << ":" << level << " to plan" << std::endl;
	if (!found) {
		found = knownSkills_[skill->getID()];
	}
	if (found && found->getLevel() >= level) {
		//std::cerr << "Already known, skipping" << std::endl;
		return false;
	}
	if (level > 1) {
		addSkill(skill, level - 1);
	}
	SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(shared_from_this(), skill, level);
	plannedSkills_[skillLevel->getID()] = skillLevel;
	std::vector<boost::shared_ptr<SkillLevel> > dependencies = skill->getDependencies();
	foreach (SkillLevel::ref dependency, dependencies) {
		//std::cerr << "Found dependency on " << dependency->getSkill()->getName() << ":" << dependency->getLevel() << std::endl;
		addSkill(dependency->getSkill(), dependency->getLevel());
	}
	//std::cerr << "Finished dependencies" << std::endl;
	
	plan_.push_back(skillLevel);
	
	return true;
}

void SkillPlan::setKnownSkills(SkillItem::ref knownSkillRoot) {
	knownSkills_.clear();
	std::vector<SkillItem::ref> groups = knownSkillRoot->getChildren();
	foreach (SkillItem::ref group, groups) {
		std::vector<SkillItem::ref> levels = group->getChildren();
		foreach (SkillItem::ref levelItem, levels) {
			SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(levelItem);
			if (level) {
				knownSkills_[level->getID()] = level;
			}
		}
	}

	// Now recalculate the queue
	std::vector<SkillItem::ref> oldPlan = getChildren();
	plan_.clear();
	plannedSkills_.clear();
	foreach (SkillItem::ref item, oldPlan) {
		SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(item);
		addSkill(level->getSkill(), level->getLevel());
	}
}

std::vector<SkillItem::ref> SkillPlan::getChildren() const {
	return plan_;
}

}

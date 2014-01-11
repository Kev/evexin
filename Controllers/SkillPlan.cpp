/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlan.h>

#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {

SkillPlan::SkillPlan(SkillItem::ref parent, const std::string& id, const std::string& name, SkillTree::ref allSkills) : SkillItem(parent, id, name), allSkills_(allSkills) {

}

SkillPlan::~SkillPlan() {

}

SkillPlan::SkillPlan(const SkillPlan& other) : SkillItem(other.getParent(), other.getID(), other.getName()), knownSkills_(other.knownSkills_), plannedSkills_(other.plannedSkills_), plan_(other.plan_), allSkills_(other.allSkills_) {

}

bool SkillPlan::addSkill(Skill::ref skill) {
	int level = 5;
	return addSkill(skill, level);
}

bool SkillPlan::addSkill(Skill::ref skill, int level) {
	SkillLevel::ref found = plannedSkills_[skill->getID()];
	// std::cerr << "Adding " << skill->getName() << ":" << level << " to plan" << std::endl;
	if (!found) {
		found = knownSkills_[skill->getID()];
	}
	if (found && found->getLevel() >= level) {
		// std::cerr << "Already known, skipping" << std::endl;
		return false;
	}
	if (level > 1) {
		addSkill(skill, level - 1);
	}
	SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(shared_from_this(), skill, level);
	plannedSkills_[skillLevel->getID()] = skillLevel;
	std::vector<boost::shared_ptr<SkillLevel> > dependencies = skill->getDependencies();
	foreach (SkillLevel::ref dependency, dependencies) {
		// std::cerr << "Found dependency on " << dependency->getSkill()->getName() << ":" << dependency->getLevel() << std::endl;
		addSkill(dependency->getSkill(), dependency->getLevel());
	}
	// std::cerr << "Finished dependencies" << std::endl;
	
	plan_.push_back(skillLevel);
	
	return true;
}

bool SkillPlan::addSkill(const std::string& skillID, int level, size_t position) {
	SkillLevel::ref known = knownSkills_[skillID];
	int knownLevel = known ? known->getLevel() : -1;
	if (level > 0 && knownLevel >= level) {
		return false;
	}
	SkillLevel::ref planned = plannedSkills_[skillID];
	int plannedLevel = planned ? planned->getLevel() : -1;
	if (level == -1) {
		// They're asking us to insert the next level
		level = 1;
		if (knownLevel > 0) {
			level = knownLevel + 1;
		}
		if (plannedLevel > 0) {
			level = plannedLevel + 1;
		}
	}
	if (level > 5) {
		return false;
	}
	std::vector<SkillItem::ref> oldPlan = plan_;
	clear();
	for (size_t i = 0; i < oldPlan.size() && i < position; i++) {
		SkillLevel::ref skillLevel = boost::dynamic_pointer_cast<SkillLevel>(oldPlan[i]);
		if (!skillLevel) continue;
		if (skillLevel->getSkill()->getID() == skillID && skillLevel->getLevel() >= level) {
			// So if we're trying to move a skill later in the list, we need to not
			// repopulate the requested level, or any higher level
			continue;
		}
		addSkill(skillLevel->getSkill(), skillLevel->getLevel());
	}
	addSkill(allSkills_->getSkill(skillID), level);
	for (size_t i = 0; i < oldPlan.size(); i++) {
		// Start from the top again, because we might have e.g. delayed adding requestedSkill:4
		// because they asked to move requestedSkill:3 later in the list
		SkillLevel::ref skillLevel = boost::dynamic_pointer_cast<SkillLevel>(oldPlan[i]);
		if (!skillLevel) continue;
		addSkill(skillLevel->getSkill(), skillLevel->getLevel());
	}
	return true;
}

void SkillPlan::clear() {
	plan_.clear();
	plannedSkills_.clear();	
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
	clear();
	foreach (SkillItem::ref item, oldPlan) {
		SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(item);
		addSkill(level->getSkill(), level->getLevel());
	}
}

std::vector<SkillItem::ref> SkillPlan::getChildren() const {
	return plan_;
}
	
}

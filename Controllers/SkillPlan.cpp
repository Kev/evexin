/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlan.h>

#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {

SkillPlan::SkillPlan(SkillItem::ref parent, const std::string& id, const std::string& name, SkillTree::ref allSkills) : SkillItem(parent, id, name), allSkills_(allSkills), addDepth_(0), savingDisabled_(false), debug_(false), debugDepth_(0) {

}

SkillPlan::~SkillPlan() {

}

SkillPlan::SkillPlan(const SkillPlan& other) : SkillItem(other.getParent(), other.getID(), other.getName()), knownSkills_(other.knownSkills_), plannedSkills_(other.plannedSkills_), plan_(other.plan_), allSkills_(other.allSkills_) {

}

bool SkillPlan::addSkill(Skill::ref skill) {
	int level = 5;
	return addSkill(skill, level);
}

void SkillPlan::aboutToAdd() {
	if (addDepth_ == 0 && !savingDisabled_) {
		pushUndoState();
	}
	addDepth_++;
}

void SkillPlan::addFinished() {
	addDepth_--;
	if (addDepth_ == 0 && !savingDisabled_) {
		save();
	}
}

bool SkillPlan::addSkill(Skill::ref skill, int level) {
	++debugDepth_;
	if (debug_) std::cerr << "D" << debugDepth_ << " Adding Skill " << skill->getName() << ":" << level << std::endl;
	SkillLevel::ref found = plannedSkills_[skill->getID()];
	if (!found) {
		found = knownSkills_[skill->getID()];
	}
	if (found && found->getLevel() >= level) {
		if (debug_) std::cerr << "D" << debugDepth_ << " Present or known" << std::endl;
		--debugDepth_;
		return true;
	}

	if (level > 1) {
		addSkill(skill, level - 1);
	}
	foreach (auto blockedItem, blackList_) {
		if (skill->getID() == blockedItem.first && level >= blockedItem.second) {
			if (debug_) std::cerr << "D" << debugDepth_ << " Blocked" << std::endl;
			--debugDepth_;
			return false;
		}
	}
	SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(shared_from_this(), skill, level);
	SkillLevel::ref oldPlannedLevel = plannedSkills_[skillLevel->getID()];
	plannedSkills_[skillLevel->getID()] = skillLevel; // Do this to avoid trying to add dependencies multiple times
	std::vector<boost::shared_ptr<SkillLevel> > dependencies = skill->getDependencies();
	foreach (SkillLevel::ref dependency, dependencies) {
		if (debug_) std::cerr << "D" << debugDepth_ << " Dependency " << dependency->getSkill()->getName() << ":" << dependency->getLevel() << std::endl;
		if (!addSkill(dependency->getSkill(), dependency->getLevel())) {
			if (debug_) std::cerr << "D" << debugDepth_ << " Dependency Blocked" << std::endl;
			--debugDepth_;
			plannedSkills_[skillLevel->getID()] = oldPlannedLevel; // Now remove because we can't train dependencies
			return false;
		}
	}
	plan_.push_back(skillLevel);
	--debugDepth_;
	return true;
}

bool SkillPlan::addSkill(const std::string& skillID, int level, size_t position) {
	SkillLevel::ref known = knownSkills_[skillID];
	int knownLevel = known ? known->getLevel() : -1;
	if (level > 0 && knownLevel >= level) {
		return true;
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
	aboutToAdd();
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
	addFinished();
	return true;
}

void SkillPlan::removeSkill(const std::string& skillID, int level) {
	aboutToAdd();
	blackList_.push_back(std::pair<std::string, int>(skillID, level));
	std::vector<SkillItem::ref> oldPlan = plan_;
	clear();
	foreach (SkillItem::ref item, oldPlan) {
		SkillLevel::ref skillLevel = boost::dynamic_pointer_cast<SkillLevel>(item);
		if (!skillLevel) continue;
		addSkill(skillLevel->getSkill(), skillLevel->getLevel());
	}
	blackList_.clear();
	addFinished();
}

void SkillPlan::clear() {
	if (debug_) std::cerr << "Clearing plan " << getName() << std::endl;
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
				if (!level->isComplete()) {
					level = boost::make_shared<SkillLevel>(level->getParent(), level->getSkill(), level->getLevel() - 1, -1);
				}
				if (level->getLevel() >= 1) {
					knownSkills_[level->getID()] = level;
				}
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

void SkillPlan::pushUndoState() {
	while (undoStates_.size() > 30) {
		undoStates_.pop_front();
	}
	undoStates_.push_back(plan_);
}

void SkillPlan::undo() {
	if (undoStates_.empty()) {
		return;
	}
	plan_ = undoStates_.back();
	undoStates_.pop_back();
}

void SkillPlan::save() {
	onWantsToSave();
}

void SkillPlan::disableSaving() {
	savingDisabled_ = true;
}

void SkillPlan::enableSaving() {
	savingDisabled_ = false;
}

}

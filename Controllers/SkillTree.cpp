/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillTree.h>


#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {

SkillTree::SkillTree() : SkillItem(SkillItem::ref(), "root", "root") {

}

SkillTree::~SkillTree() {

}

void SkillTree::addChild(SkillItem::ref group) {
	SkillItem::addChild(group);
	groups_[group->getID()] = group;
}

void SkillTree::addSkill(Skill::ref skill) {
	skills_[skill->getID()] = skill;
}

Skill::ref SkillTree::getSkill(const std::string& id) {
	Skill::ref skill = skills_[id];
	if (!skill) {
		/* So that if we get asked for a skill before it's been populated,
		 * we return the same pointer that'll later contain the data.
		 *
		 * This happens if you have a skill with dependencies on skills 
		 * that come later.
		 */ 
		skill = boost::make_shared<Skill>(id);
		skills_[id] = skill;
	}
	return skill;
}

SkillItem::ref SkillTree::getGroup(const std::string& id) {
	SkillItem::ref group = groups_[id];
	return group;
}

SkillItem::ref SkillTree::mergeWithCharacterSkills(boost::shared_ptr<Character> character) {
	SkillItem::ref merged = boost::make_shared<SkillItem>(SkillItem::ref(), "merged_root", "merged_root");
	foreach (auto groupPair, groups_) {
		if (!groupPair.second) {
			//FIXME: Find what's inserting a "" null group in the tree
			continue;
		}
		std::vector<SkillItem::ref> children = groupPair.second->getChildren();
		SkillItem::ref groupItem = boost::make_shared<SkillItem>(merged, groupPair.second->getID(), groupPair.second->getName());
		merged->addChild(groupItem);
		SkillItem::ref characterGroup;
		if (character) {
			SkillItem::ref known = character->getKnownSkills();
			if (known) {
				characterGroup = known->getChild(groupItem->getID());
			}
		}
		foreach (SkillItem::ref skillItem, children) {
			//std::cerr << "Char:" << character->getID() << ": Source item " << groupItem->getName() << "::" << skillItem->getName() << std::endl;
			SkillItem::ref newSkill;
			SkillLevel::ref level = characterGroup ? boost::dynamic_pointer_cast<SkillLevel>(characterGroup->getChild(skillItem->getID())) : SkillLevel::ref();
			if (level) {
				newSkill = boost::make_shared<SkillLevel>(groupItem, level->getSkill(), level->getLevel());
			}
			else {
				if (skillItem->getSkill()) {
					newSkill = boost::make_shared<SkillItem>(groupItem, skillItem->getSkill());
				}
				else {
					newSkill = boost::make_shared<SkillItem>(groupItem, skillItem->getID(), skillItem->getName());
				}
			}
			groupItem->addChild(newSkill);
		}
	}
	return merged;
}

}

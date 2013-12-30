/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <Eve-Xin/Controllers/Skill.h>
#include <Eve-Xin/Controllers/SkillItem.h>

namespace EveXin {

	class SkillTree : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillTree> ref;

			SkillTree() : SkillItem(SkillItem::ref(), "root", "root") {}
			virtual ~SkillTree() {}

			virtual void addChild(SkillItem::ref group) {
				SkillItem::addChild(group);
				groups_[group->getID()] = group;
			}

			void addSkill(Skill::ref skill) {
				skills_[skill->getID()] = skill;
			}

			Skill::ref getSkill(const std::string& id) {
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

			SkillItem::ref getGroup(const std::string& id) {
				SkillItem::ref group = groups_[id];
				return group;
			}
		private:
			std::map<std::string, SkillItem::ref> groups_;
			std::map<std::string, Skill::ref> skills_;
			SkillItem::ref dummyGroup_; // Things get put in this if they're created before being populated
	};
}

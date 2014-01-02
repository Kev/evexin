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
	class Character;
	class SkillTree : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillTree> ref;

			SkillTree();
			virtual ~SkillTree();

			virtual void addChild(SkillItem::ref group);

			void addSkill(Skill::ref skill);

			Skill::ref getSkill(const std::string& id);

			SkillItem::ref getGroup(const std::string& id);

			SkillItem::ref mergeWithCharacterSkills(boost::shared_ptr<Character> character);
		private:
			std::map<std::string, SkillItem::ref> groups_;
			std::map<std::string, Skill::ref> skills_;
			SkillItem::ref dummyGroup_; // Things get put in this if they're created before being populated
	};
}

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
#include <Eve-Xin/Controllers/SkillGroup.h>

namespace EveXin {

	class SkillTree {
		public:
			typedef boost::shared_ptr<SkillTree> ref;
			void addGroup(boost::shared_ptr<SkillGroup> group) {
				groups_[group->getID()] = group;
			}

			void addSkill(boost::shared_ptr<Skill> skill) {
				skills_[skill->getID()] = skill;
			}

			boost::shared_ptr<Skill> getSkill(const std::string& id) {
				boost::shared_ptr<Skill> skill = skills_[id];
				if (!skill) {
					skill = boost::make_shared<Skill>();
				}
				return skill;
			}
		private:
			std::map<std::string, boost::shared_ptr<SkillGroup> > groups_;
			std::map<std::string, boost::shared_ptr<Skill> > skills_;
	};
}

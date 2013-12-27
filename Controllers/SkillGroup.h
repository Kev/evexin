/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <Eve-Xin/Controllers/Skill.h>

namespace EveXin {
	class SkillGroup : public boost::enable_shared_from_this<SkillGroup> {
		public:
			typedef boost::shared_ptr<SkillGroup> ref;
			SkillGroup(const std::string& id, const std::string& name) : id_(id), name_(name) {}
			void addSkill(Skill::ref skill) {
				skills_.push_back(skill);
				skill->setGroupRef(shared_from_this());
			}

			std::string getID() {
				return id_;
			}

			std::string getName() {
				return name_;
			}

		private:
			std::string id_;
			std::string name_;
			std::vector<Skill::ref> skills_;
	};
}

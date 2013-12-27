/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <boost/enable_shared_from_this.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>

namespace EveXin {
	class SkillGroup : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillGroup> ref;
			SkillGroup() {}
			SkillGroup(const std::string& id, const std::string& name) : id_(id), name_(name) {}
			void addChild(SkillItem::ref skill) {
				children_.push_back(skill);
			}

			std::string getID() {
				return id_;
			}

			std::string getName() {
				return name_;
			}

			const std::vector<SkillItem::ref>& getChildren() {
				return children_;
			}

		private:
			std::string id_;
			std::string name_;
			std::vector<SkillItem::ref> children_;
	};
}

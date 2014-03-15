/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>

namespace EveXin {
	class SkillOrderedList : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillOrderedList> ref;

			SkillOrderedList(boost::shared_ptr<SkillItem> parent, const std::string& id, const std::string& name);
			~SkillOrderedList();

			virtual std::vector<SkillItem::ref> getChildren() const;

			void addChild(SkillItem::ref child);

		private:
			std::vector<SkillItem::ref> children_;
	};
}

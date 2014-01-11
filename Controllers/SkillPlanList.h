/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillPlan.h>
#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {
	class SkillPlanList : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillPlanList> ref;

			SkillPlanList(const std::string id, const std::string& name, boost::shared_ptr<SkillTree> allSkills);
			virtual ~SkillPlanList();

		private:
			boost::shared_ptr<SkillTree> allSkills_;
	};
}

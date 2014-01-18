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

#include <Swiften/Base/boost_bsignals.h>

#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillPlan.h>
#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {
	class SkillPlanList : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillPlanList> ref;

			SkillPlanList(const std::string id, const std::string& name, boost::shared_ptr<SkillTree> allSkills);
			virtual ~SkillPlanList();

			SkillPlan::ref createPlan(const std::string& name);
			void deletePlan(SkillPlan::ref plan);
		public:
			/**
			 * The plan is null when deleting.
			 */
			boost::signal<void(SkillPlan::ref)> onWantsToSave;
		private:
			void handleSkillPlanWantsToSave(SkillPlan::ref plan);
		private:
			boost::shared_ptr<SkillTree> allSkills_;
			int nextID_;
	};
}

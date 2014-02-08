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

			SkillPlan::ref createPlan(const std::string& name, bool userAction = true);
			void deletePlan(SkillPlan::ref plan, bool userAction = true);

			void setKnownSkills(boost::shared_ptr<SkillItem> knownSkills);

			void undo();
		public:
			/**
			 * The plan is null when deleting.
			 */
			boost::signal<void(SkillPlan::ref)> onWantsToSave;
			/**
			 * Only called for a single change - either one add or one delete.
			 */
			boost::signal<void()> onAvailablePlansChanged;
		private:
			enum UndoAction {CreatePlan, DeletePlan, ModifyPlan};
			void handleSkillPlanWantsToSave(SkillPlan::ref plan);
			void addUndoAction(const std::pair<UndoAction, SkillPlan::ref>&, bool userAction);
		private:
			boost::shared_ptr<SkillTree> allSkills_;
			int nextID_;
			std::vector<std::pair<UndoAction, SkillPlan::ref > > undoActions_;
			bool undoing_;
			boost::shared_ptr<SkillItem> knownSkills_;
	};
}

/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <deque>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>

#include <Eve-Xin/Controllers/Skill.h>
#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {
	class SkillTree;
	class SkillPlan : public SkillItem {
		public:
			typedef boost::shared_ptr<SkillPlan> ref;

			SkillPlan(SkillItem::ref parent, const std::string& id, const std::string& name, boost::shared_ptr<SkillTree> allSkills);
			SkillPlan(const SkillPlan& other);
			virtual ~SkillPlan();

			/**
			 * Try to add another level of this skill to the plan.
			 * Will insert dependencies.
			 * @return  Skill added.
			 */
			bool addSkill(Skill::ref skill);

			/**
			 * Try to add another level of the skill in the given position.
			 * This may cause significant jiggery-pokery as the dependencies get
			 * re-ordered, and the final index may well not be the provided
			 * position, if more dependencies had to be injected beforehand.
			 * Will clean up any later dependencies.
			 *
			 * For this one, passing -1 as level will try to add the next level.
			 * Passing a non -1 level will try to move the existing skill elsewhere
			 * in the list, if it matches an existing item.
			 */
			bool addSkill(const std::string& skillID, int level, size_t position);
			
			/**
			 * Try to add specified level of this skill to the plan.
			 * Will insert dependencies.
			 * Safe to call with skills that are already trained, just returns true.
			 * Will ignore any skills in the blacklist.
			 * @return  Skill added or already present.
			 */
			bool addSkill(Skill::ref skill, int level);

			/**
			 * Set the known skills for this character.
			 * Will remove any skills from the plan that are now known.
			 * Iterates down the tree from the provided root.
			 */
			void setKnownSkills(SkillItem::ref knownSkillRoot);

			virtual std::vector<SkillItem::ref> getChildren() const;

			/**
			 * Remove the skill, and anything that has it as a dependency
			 */
			void removeSkill(const std::string& skillID, int level);

			/**
			 * Clear the plan out.
			 */
			void clear();

			/**
			 * Undoes a change
			 */
			void undo();


			void disableSaving();
			void enableSaving();

			void setDebug(bool debug) {debug_ = debug;}
		public:
			boost::signal<void()> onWantsToSave;
		private:
			void pushUndoState();
			void aboutToAdd();
			void addFinished();
			void save();

		private:
			std::map<std::string, SkillLevel::ref> knownSkills_; // Flat mapping of known skill levels
			std::map<std::string, SkillLevel::ref> plannedSkills_; // Flat mapping of known skill levels
			std::vector<SkillItem::ref> plan_;
			boost::shared_ptr<SkillTree> allSkills_;
			std::deque<std::vector<SkillItem::ref> > undoStates_;
			int addDepth_;
			bool savingDisabled_;
			std::vector<std::pair<std::string, int> > blackList_;
			bool debug_;
			int debugDepth_;
	};
}

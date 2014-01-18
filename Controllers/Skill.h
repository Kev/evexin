/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <Eve-Xin/Controllers/SkillAttribute.h>

namespace EveXin {
	class SkillLevel;
	class Skill {
		public:
			typedef boost::shared_ptr<Skill> ref;
			Skill(const std::string& id);
			~Skill();

			/**
			 * Use this rather than a ctor, as it allows keeping smart pointers to not-yet-populated skills in dependencies.
			 */
			void populate(const std::string& groupID_, /*boost::weak_ptr<SkillItem> group, */const std::string& name, const std::string& description, int rank, SkillAttribute::Attribute primaryAttribute, SkillAttribute::Attribute secondaryAttribute, const std::vector<boost::shared_ptr<SkillLevel> >& dependencies);

			std::string getID() {return id_;}
			std::string getGroupID() {return groupID_;}

			std::string getName() {return name_;}
			std::string getDescription() {return description_;}
			int getRank() {return rank_;}
			SkillAttribute::Attribute getPrimaryAttribute() {return primaryAttribute_;}
			SkillAttribute::Attribute getSecondaryAttribute() {return secondaryAttribute_;}
			std::vector<boost::shared_ptr<SkillLevel> > getDependencies() {return dependencies_;}

			/**
			 * Return the 'real' group that this skill belongs to.
			 * This is the main skill tree, not any skill plans.
			 */
			//boost::shared_ptr<SkillItem> getGroup() {return groupRef_.lock();}
		private:
			std::string id_;
			std::string groupID_;
			//boost::weak_ptr<SkillItem> groupRef_;
			std::string name_;
			std::string description_;
			int rank_;
			SkillAttribute::Attribute primaryAttribute_;
			SkillAttribute::Attribute secondaryAttribute_;
			std::vector<boost::shared_ptr<SkillLevel> > dependencies_;
	};
}


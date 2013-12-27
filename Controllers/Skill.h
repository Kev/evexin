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

#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {
	class SkillGroup;
	class Skill {
		public:
			typedef boost::shared_ptr<Skill> ref;
			Skill();
			~Skill();

			/**
			 * Use this rather than a ctor, as it allows keeping smart pointers to not-yet-populated skills in dependencies.
			 */
			void populate(const std::string& id, const std::string& groupID_, const std::string& name, const std::string& description, int rank, const std::string& primaryAttribute, const std::string& secondaryAttribute, const std::vector<SkillLevel>& dependencies);
			void setGroupRef(boost::weak_ptr<SkillGroup>);

			std::string getID() {return id_;}
			std::string getGroupID() {return groupID_;}
			boost::weak_ptr<SkillGroup> getGroup() {return groupRef_;}
			std::string getName() {return name_;}
			std::string getDescription() {return description_;}
			int getRank() {return rank_;}
			std::string getPrimaryAttribute() {return primaryAttribute_;}
			std::string getSecondaryAttribute() {return secondaryAttribute_;}
			std::vector<SkillLevel> getDependencies() {return dependencies_;}
		private:
			std::string id_;
			std::string groupID_;
			boost::weak_ptr<SkillGroup> groupRef_;
			std::string name_;
			std::string description_;
			int rank_;
			std::string primaryAttribute_;
			std::string secondaryAttribute_;
			std::vector<SkillLevel> dependencies_;
	};
}


/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/Skill.h>


namespace EveXin {

Skill::Skill() {

}

Skill::~Skill() {

}

void Skill::populate(const std::string& id, const std::string& groupID, const std::string& name, const std::string& description, int rank, const std::string& primaryAttribute, const std::string& secondaryAttribute, const std::vector<SkillLevel>& dependencies) {
	id_ = id;
	groupID_ = groupID;
	name_ = name;
	description_ = description;
	rank_ = rank;
	primaryAttribute_ = primaryAttribute;
	secondaryAttribute_ = secondaryAttribute;
	dependencies_ = dependencies;
}

void Skill::setGroupRef(boost::weak_ptr<SkillGroup> groupRef) {
	groupRef_ = groupRef;
}

}

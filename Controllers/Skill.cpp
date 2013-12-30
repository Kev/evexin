/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/Skill.h>


namespace EveXin {

Skill::Skill(const std::string& id) : id_(id) {

}

Skill::~Skill() {

}

void Skill::populate(const std::string& groupID, /*boost::weak_ptr<SkillItem> group, */const std::string& name, const std::string& description, int rank, const std::string& primaryAttribute, const std::string& secondaryAttribute, const std::vector<boost::shared_ptr<SkillLevel> >& dependencies) {
	groupID_ = groupID;
	//group_ = group;
	name_ = name;
	description_ = description;
	rank_ = rank;
	primaryAttribute_ = primaryAttribute;
	secondaryAttribute_ = secondaryAttribute;
	dependencies_ = dependencies;
}

}

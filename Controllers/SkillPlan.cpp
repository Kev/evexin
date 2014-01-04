/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillPlan.h>

namespace EveXin {

SkillPlan::SkillPlan(SkillItem::ref parent, const std::string& id, const std::string& name) : SkillItem(parent, id, name) {

}

SkillPlan::~SkillPlan() {

}

bool SkillPlan::addSkill(Skill::ref skill) {

}

bool SkillPlan::addSkill(Skill::ref skill, int level) {

}

void SkillPlan::setKnownSkills(SkillItem::ref knownSkillRoot) {

}

}

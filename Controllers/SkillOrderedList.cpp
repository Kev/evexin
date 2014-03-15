/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SkillOrderedList.h>


namespace EveXin {

SkillOrderedList::SkillOrderedList(SkillItem::ref parent, const std::string& id, const std::string& name) : SkillItem(parent, id, name) {

}

SkillOrderedList::~SkillOrderedList() {

}

std::vector<SkillItem::ref> SkillOrderedList::getChildren() const {
	return children_;
}

void SkillOrderedList::addChild(SkillItem::ref child) {
	children_.push_back(child);
}

}

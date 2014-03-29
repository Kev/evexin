/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/Character.h>

#include <boost/bind.hpp>

#include <Eve-Xin/Controllers/SkillPlanList.h>

namespace EveXin {

Character::Character(const std::string& id, const std::string& name, const std::string& accountKey, const std::string& corpKey, const std::string& corpName, const std::string& expires) : id_(id), name_(name), accountKey_(accountKey), corpKey_(corpKey), corpName_(corpName), expires_(expires), isk_(0.0) {

}

Character::~Character() {
	if (skillPlanRoot_) {
		skillPlanRoot_->onWantsToSave.disconnect(boost::bind(&Character::injectTrainingIntoPlan, this));
	}
}

void Character::setImplants(const std::map<SkillAttribute::Attribute, std::string>& enhancerNames, const std::map<SkillAttribute::Attribute, int>& enhancerValues) {
	enhancerNames_ = enhancerNames;
	enhancerValues_ = enhancerValues;
	onDataChanged();
}

std::string Character::getImplantName(SkillAttribute::Attribute attribute) {
	return enhancerNames_[attribute];
}

int Character::getImplantValue(SkillAttribute::Attribute attribute) {
	auto it = enhancerValues_.find(attribute);
	return (it != enhancerValues_.end()) ? (*it).second : 0;
}

void Character::setAvatar(size_t size, const Swift::ByteArray& avatar) {
	avatars_[size] = avatar;
	onDataChanged();
}

Swift::ByteArray Character::getAvatar(size_t size) {
	return avatars_[size];
}

void Character::setKnownSkills(boost::shared_ptr<SkillItem> skillRoot) {
	knownSkillRoot_ = skillRoot;
	if (skillPlanRoot_) {
		skillPlanRoot_->setKnownSkills(knownSkillRoot_);
	}
	onDataChanged();
}

void Character::setTrainingQueue(boost::shared_ptr<SkillItem> trainingQueue) {
	trainingQueueRoot_ = trainingQueue;
	injectTrainingIntoPlan();
	onDataChanged();
}


void Character::setSkillPlanRoot(boost::shared_ptr<SkillPlanList> skillPlanRoot) {
	if (skillPlanRoot_) {
		skillPlanRoot_->onWantsToSave.disconnect(boost::bind(&Character::injectTrainingIntoPlan, this));
	}
	skillPlanRoot_ = skillPlanRoot;
	skillPlanRoot_->onWantsToSave.connect(boost::bind(&Character::injectTrainingIntoPlan, this));
	if (knownSkillRoot_) {
		skillPlanRoot_->setKnownSkills(knownSkillRoot_);
	}
	injectTrainingIntoPlan();
	onDataChanged();
}

void Character::injectTrainingIntoPlan() {
	if (trainingQueueRoot_ && skillPlanRoot_) {
		std::vector<SkillItem::ref> trainingQueue = trainingQueueRoot_->getChildren()[0]->getChildren();
		if (trainingQueue.size() > 0) {
			SkillLevel::ref trainingSkill = boost::dynamic_pointer_cast<SkillLevel>(trainingQueue[0]);
			std::vector<SkillItem::ref> plans = skillPlanRoot_->getChildren();
			foreach(SkillItem::ref planItem, plans) {
				SkillPlan::ref plan = boost::dynamic_pointer_cast<SkillPlan>(planItem);
				plan->updateLevel(trainingSkill);
			}
		}
	}
}

void Character::setAttribute(SkillAttribute::Attribute attribute, int value) {
	baseAttributes_[attribute] = value;
	onDataChanged();
}

void Character::setISK(double isk) {
	isk_ = isk;
	onDataChanged();
}

}



/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Eve-Xin/Controllers/Character.h>
#include <Eve-Xin/Controllers/SkillLevel.h>

namespace EveXin {
	class SkillTime {
		public:
			static float minutesToTrain(Character::ref character, SkillLevel::ref skillLevel) {
				boost::posix_time::ptime startTime = skillLevel->getStartTime();
				boost::posix_time::ptime endTime = skillLevel->getEndTime();
				if (startTime != boost::posix_time::not_a_date_time) {
					// For skills in training, we have the datetimes we can use
					boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
					if (now >= startTime) {
						boost::posix_time::time_duration remaining = endTime - now;
						return remaining.total_seconds() / 60.0;
					}
				}

				// If not already training, we'll have to calculate by hand
				int pointsNeeded = skillLevel->getRemainingSkillPoints();
				SkillAttribute::Attribute primaryAttribute = skillLevel->getSkill()->getPrimaryAttribute();
				int primaryAttributeValue = character->getAttribute(primaryAttribute);
				SkillAttribute::Attribute secondaryAttribute = skillLevel->getSkill()->getSecondaryAttribute();
				int secondaryAttributeValue = character->getAttribute(secondaryAttribute);
				int primaryAttributeImplant = character->getImplantValue(primaryAttribute);
				int secondaryAttributeImplant = character->getImplantValue(secondaryAttribute);
				float attributeModifier = (primaryAttributeValue + primaryAttributeImplant + (secondaryAttributeValue + secondaryAttributeImplant) / 2.0);
				if (attributeModifier == 0) {
					// Something has gone badly wrong. Perhaps corrupt character data,
					// or skills that don't exist any more. Regardless, let's not blow up.
					std::cerr << "WARNING: attribute modifier error" << std::endl;
					attributeModifier = 1;
				}
				float minutes = pointsNeeded / attributeModifier;
				return minutes;
			}

			static boost::posix_time::ptime timeToTrain(Character::ref character, SkillLevel::ref skillLevel) {
				boost::posix_time::ptime startTime = boost::posix_time::second_clock::local_time();
				return startTime + boost::posix_time::minutes(minutesToTrain(character, skillLevel));
			}

			static float minutesToTrainAll(Character::ref character, SkillItem::ref item) {
				SkillLevel::ref level;
				if ((level = boost::dynamic_pointer_cast<SkillLevel>(item))) {
					return minutesToTrain(character, level);
				}
				float total = 0;
				std::vector<SkillItem::ref> children = item->getChildren();
				foreach (SkillItem::ref child, children) {
					total += minutesToTrainAll(character, child);
				}
				return total;
			}

			static std::pair<float, std::map<SkillAttribute::Attribute, int> > suggestAttributes(SkillPlan::ref plan) {
				std::map<SkillAttribute::Attribute, int> attributes;
				std::vector<SkillAttribute::Attribute> allAttributes = SkillAttribute::allAttributes();
				foreach (auto attribute, allAttributes) {
					attributes[attribute] = 17;
				}
				int remaining = 20 * 4 + 19 - 5 * 17;
				float bestMinutes = FLT_MAX;
				while (remaining > 0) {
					SkillAttribute::Attribute bestAttribute = SkillAttribute::Unknown;
					bestMinutes = FLT_MAX;
					foreach (auto attribute, allAttributes) {
						if (attributes[attribute] < 27) {
							Character::ref fakeCharacter = boost::make_shared<Character>("", "", "", "", "", "");
							foreach (auto charAttribute, allAttributes) {
								int value = attributes[charAttribute];
								if (charAttribute == attribute) {
									value++;
								}
								fakeCharacter->setAttribute(charAttribute, value);
							}
							float minutes = minutesToTrainAll(fakeCharacter, plan);
							if (minutes < bestMinutes) {
								bestAttribute = attribute;
								bestMinutes = minutes;
							}
						}
					}
					remaining--;
					attributes[bestAttribute] = attributes[bestAttribute] + 1;
				}
				return std::pair<float, std::map<SkillAttribute::Attribute, int> >(bestMinutes, attributes);
			}
	};
};









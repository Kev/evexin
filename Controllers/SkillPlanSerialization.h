/*
 * Copyright (c) 2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/foreach.h>

#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillLevel.h>
#include <Eve-Xin/Controllers/SkillPlan.h>
#include <Eve-Xin/Controllers/SkillPlanList.h>
#include <Eve-Xin/Controllers/SkillTree.h>



namespace EveXin {
	class SkillPlanSerialization {
		private:
			class SimpleSkill {
				private:
					template<class Archive>
					void serialize(Archive & ar, const unsigned int /*version*/) {
						ar & id_;
						ar & level_;
					}
					friend class boost::serialization::access;
				public:
					std::string id_;
					int level_;
			};


			class SimpleSkillPlan {
				private:
					template<class Archive>
					void serialize(Archive & ar, const unsigned int /*version*/) {
						ar & name_;
						ar & skills_;
					}
					friend class boost::serialization::access;
				public:
					std::string name_;
					std::vector<boost::shared_ptr<SimpleSkill> > skills_;
			};

			class SimpleSkillPlanList {
				private:
					template<class Archive>
					void serialize(Archive & ar, const unsigned int /*version*/) {
						ar & plans_;
					}
					friend class boost::serialization::access;
				public:
					std::vector<boost::shared_ptr<SimpleSkillPlan> > plans_;
			};

		public:
			static SkillPlanList::ref parseSkills(const Swift::ByteArray& skillBytes, SkillTree::ref skillTree, SkillItem::ref knownSkillRoot) {
				SimpleSkillPlanList simpleList;
				std::string string = Swift::byteArrayToString(skillBytes);
				// std::cerr << "Parsing " << string << std::endl;
				try {
					std::stringstream inputStream;
					inputStream << string;
					boost::archive::text_iarchive inputArchive(inputStream);
					inputArchive >> simpleList;
				} catch(const boost::archive::archive_exception& ex) {
					std::cerr << "Error parsing saved skills: " << ex.what() << std::endl;
				} catch(...) {
					std::cerr << "Error parsing saved skills" << std::endl;
				}

				SkillPlanList::ref result = boost::make_shared<SkillPlanList>("planroot", "planroot", skillTree);
				foreach (auto simplePlan, simpleList.plans_) {
					SkillPlan::ref plan = result->createPlan(simplePlan->name_, false);
					plan->setKnownSkills(knownSkillRoot);
					foreach (auto simpleSkill, simplePlan->skills_) {
						plan->addSkill(skillTree->getSkill(simpleSkill->id_), simpleSkill->level_);
					}
				}
				return result;
			}

			static Swift::ByteArray serializeSkills(SkillPlanList::ref skills) {
				SimpleSkillPlanList simpleList;

				auto lists = skills->getChildren();
				foreach (SkillItem::ref list, lists) {
					boost::shared_ptr<SimpleSkillPlan> simplePlan = boost::make_shared<SimpleSkillPlan>();
					simplePlan->name_ = list->getName();
					auto items = list->getChildren();
					foreach (SkillItem::ref item, items) {
						SkillLevel::ref level = boost::dynamic_pointer_cast<SkillLevel>(item);
						boost::shared_ptr<SimpleSkill> simpleSkill = boost::make_shared<SimpleSkill>();
						simpleSkill->level_ = level->getLevel();
						simpleSkill->id_ = level->getID();
						simplePlan->skills_.push_back(simpleSkill);
					}
					simpleList.plans_.push_back(simplePlan);
				}

				std::stringstream outputStream;
				boost::archive::text_oarchive outputArchive(outputStream);
				outputArchive << simpleList;
				std::string result = outputStream.str();
				// std::cerr << "Serialized " << result << std::endl;
				return Swift::createByteArray(result);
			}
		
	};
}

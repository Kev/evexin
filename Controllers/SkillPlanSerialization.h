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
				private:
					std::string id_;
					int level_;
					friend class boost::serialization::access;
			};


			class SimpleSkillPlan {
				private:
					template<class Archive>
					void serialize(Archive & ar, const unsigned int /*version*/) {
						ar & name_;
						ar & skills_;
					}
				private:
					std::string name_;
					std::vector<boost::shared_ptr<SimpleSkill> > skills_;
					friend class boost::serialization::access;
			};

			class SimpleSkillPlanList {
				private:
					template<class Archive>
					void serialize(Archive & ar, const unsigned int /*version*/) {
						ar & plans_;
					}
				private:
					std::vector<boost::shared_ptr<SimpleSkillPlan> > plans_;
					friend class boost::serialization::access;
			};

		public:
			static SkillPlanList::ref parseSkills(const Swift::ByteArray& skillBytes, SkillTree::ref skillTree) {
				std::stringstream inputStream;
				boost::archive::text_iarchive inputArchive(inputStream);
				inputStream << Swift::byteArrayToString(skillBytes);
				SimpleSkillPlanList simpleList;
				inputArchive >> simpleList;

				SkillPlanList::ref result;
				return result;
			}

			static Swift::ByteArray serializeSkills(SkillPlanList::ref skills) {
				SimpleSkillPlanList simpleList;
				std::stringstream outputStream;
				boost::archive::text_oarchive outputArchive(outputStream);
				outputArchive << simpleList;
				return Swift::createByteArray(outputStream.str());
			}
		
	};
}

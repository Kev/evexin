/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/ByteArray.h>

#include <Eve-Xin/Controllers/SkillAttribute.h>

namespace EveXin {
	class SkillItem;
	class SkillPlanList;
	class Character {
		public:
			typedef boost::shared_ptr<Character> ref;
			Character(const std::string& id, const std::string& name, const std::string& accountKey, const std::string& corpKey, const std::string& corpName, const std::string& expires);
			~Character();

			std::string getID() {return id_;}
			std::string getName() {return name_;}
			std::string getCorpKey() {return corpKey_;}
			std::string getCorpName() {return corpName_;}
			std::string getAccountKey() {return accountKey_;}
			std::string getExpires() {return expires_;}
			void setISK(double isk);
			double getISK() {return isk_;}
			Swift::ByteArray getAvatar(size_t size);
			void setAvatar(size_t size, const Swift::ByteArray& avatar);
			void setKnownSkills(boost::shared_ptr<SkillItem> skillRoot);
			void setTrainingQueue(boost::shared_ptr<SkillItem> trainingQueue);
			void setSkillPlanRoot(boost::shared_ptr<SkillPlanList> skillPlanRoot);
			boost::shared_ptr<SkillItem> getKnownSkills() {return knownSkillRoot_;}
			boost::shared_ptr<SkillItem> getTrainingQueue() {return trainingQueueRoot_;}
			boost::shared_ptr<SkillPlanList> getSkillPlanRoot() {return skillPlanRoot_;}
			int getAttribute(SkillAttribute::Attribute attribute) {return baseAttributes_[attribute];}
			void setAttribute(SkillAttribute::Attribute attribute, int value);
			void setImplants(const std::map<SkillAttribute::Attribute, std::string>& enhancerNames, const std::map<SkillAttribute::Attribute, int>& enhancerValues);
			int getImplantValue(SkillAttribute::Attribute attribute);
			std::string getImplantName(SkillAttribute::Attribute attribute);
		public:
			boost::signal<void()> onDataChanged;
		private:
			void injectTrainingIntoPlan();
		private:
			std::string id_;
			std::string name_;
			std::string accountKey_;
			std::string corpKey_;
			std::string corpName_;
			std::string expires_;
			double isk_;
			std::map<size_t, Swift::ByteArray> avatars_;
			boost::shared_ptr<SkillItem> knownSkillRoot_;
			boost::shared_ptr<SkillItem> trainingQueueRoot_;
			boost::shared_ptr<SkillPlanList> skillPlanRoot_;
			std::map<SkillAttribute::Attribute, int> baseAttributes_;
			std::map<SkillAttribute::Attribute, int> enhancerValues_;
			std::map<SkillAttribute::Attribute, std::string> enhancerNames_;

	};
}


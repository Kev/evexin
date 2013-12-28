/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/ByteArray.h>

namespace EveXin {
	class SkillGroup;
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
			void setISK(double isk) {isk_ = isk;}
			double getISK() {return isk_;}
			Swift::ByteArray get256Avatar() {return avatar256_;}
			void set256Avatar(const Swift::ByteArray& avatar) {avatar256_ = avatar;}
			void setKnownSkills(boost::shared_ptr<SkillGroup> skillRoot) {knownSkillRoot_ = skillRoot;}
			boost::shared_ptr<SkillGroup> getKnownSkills() {return knownSkillRoot_;}
		private:
			std::string id_;
			std::string name_;
			std::string accountKey_;
			std::string corpKey_;
			std::string corpName_;
			std::string expires_;
			double isk_;
			Swift::ByteArray avatar256_;
			boost::shared_ptr<SkillGroup> knownSkillRoot_;

	};
}



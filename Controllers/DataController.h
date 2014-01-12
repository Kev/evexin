/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/URL.h>

#include <Eve-Xin/Controllers/Character.h>

namespace Swift {
	class NetworkFactories;
}

namespace EveXin {
	class DataStore;
	class GeneralResult;
	class SkillTree;
	class SkillLevel;
	class Skill;
	class SkillItem;
	class SkillPlan;
	class DataController {
		public:
			DataController(Swift::NetworkFactories* factories);
			~DataController();

			void addAPIKey(const std::string& keyID, const std::string& vCode, bool isFromCache = false);
			std::vector<std::string /*id*/> getCharacters();
			Character::ref getCharacter(const std::string& id);
			Swift::ByteArray get256CharacterAvatar(const std::string& id);
			boost::shared_ptr<SkillTree> getSkillTree();
		public:
			boost::signal<void(const std::string&)> onCharacterDataChanged;
		private:
			typedef boost::function<void (const Swift::ByteArray&)> RawCallback;
			typedef boost::function<void (boost::shared_ptr<GeneralResult>)> ParsedCallback;
			//enum ShouldSave {SaveResult, DontSave};
			Swift::ByteArray getAndCache(const Swift::URL& url, RawCallback callback);
			void handleRawResult(const Swift::URL& url, const Swift::ByteArray& content, RawCallback callback);
			void getURLandDommify(const Swift::URL& url, ParsedCallback callback);
			void handleDOMResult(const Swift::URL& url, const Swift::ByteArray& content, ParsedCallback callback);
			void handleCharactersResult(const std::string& keyID, boost::shared_ptr<GeneralResult> result);
			void handleAccountBalanceResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result);
			void handleSkillResult(boost::shared_ptr<GeneralResult> result);
			void handleCharacterSheetResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result);
			void handleCharacter256AvatarResult(const std::string& id, const Swift::ByteArray& content);
			/**
			 * Check if URL is tracked, and track if not.
			 */
			bool canRequestURL(const Swift::URL& url);
			void untrackURL(const Swift::URL& url);
			void putSkillLevelIntoRoot(boost::shared_ptr<SkillItem> root, boost::shared_ptr<SkillLevel> skillLevel);
			void loadSkillPlans(Character::ref character);
			void handleSkillPlanWantsToSave(boost::shared_ptr<SkillPlan> plan);
		private:
			Swift::NetworkFactories* factories_;
			boost::shared_ptr<DataStore> store_;
			std::map<std::string, std::string> accountKeys_;
			std::map<std::string, std::string> characterAccounts_;
			std::map<std::string /*key*/, Character::ref> characters_;
			std::set<std::string> requestsInFlight_;
			boost::shared_ptr<SkillTree> skillTree_;
	};

}

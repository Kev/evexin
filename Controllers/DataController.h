/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/URL.h>

#include <Eve-Xin/Controllers/Character.h>

namespace Swift {
	class NetworkFactories;
	class Timer;
}

namespace EveXin {
	class DataStore;
	class GeneralResult;
	class SkillTree;
	class SkillLevel;
	class Skill;
	class SkillItem;
	class SkillPlan;
	class SkillPlanList;
	class DataController {
		public:
			DataController(Swift::NetworkFactories* factories, const boost::filesystem::path&);
			~DataController();

			void addAPIKey(const std::string& keyID, const std::string& vCode, bool isFromCache = false);
			void deleteAPIKey(const std::string& keyID);
			std::vector<std::string /*keyID*/> getAPIKeys();
			/** Gets characters from this account, including ignored ones */
			std::vector<Character::ref> getCharactersForAPIKey(const std::string& apiKey);
			void setCharacterIgnored(const std::string& characterID, bool ignored);
			/** Return non-ignored characters */
			std::vector<std::string /*id*/> getCharacters();
			Character::ref getCharacter(const std::string& id);
			boost::shared_ptr<SkillTree> getSkillTree();
		public:
			boost::signal<void()> onCharacterListChanged;
			boost::signal<void()> onSkillTreeChanged;
			boost::signal<void(const std::string&)> onDebugMessage;
		private:
			typedef boost::function<void (const Swift::ByteArray&)> RawCallback;
			typedef boost::function<void (boost::shared_ptr<GeneralResult>)> ParsedCallback;
			//enum ShouldSave {SaveResult, DontSave};
			Swift::ByteArray getAndCache(const Swift::URL& url, RawCallback callback);
			void handleRawResult(const Swift::URL& url, const Swift::ByteArray& content, RawCallback callback);
			void getURLandDommify(const Swift::URL& url, ParsedCallback callback);
			void getURLandDommifySince(const Swift::URL& url, boost::posix_time::ptime since, ParsedCallback callback);
			void handleDOMResult(const Swift::URL& url, const Swift::ByteArray& content, ParsedCallback callback);
			void handleCharactersResult(const std::string& keyID, boost::shared_ptr<GeneralResult> result);
			void handleAccountBalanceResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result);
			void handleSkillResult(boost::shared_ptr<GeneralResult> result);
			void handleCharacterSheetResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result);
			void handleSkillQueueResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result);
			void handleCharacterAvatarResult(const std::string& id, size_t size, const Swift::ByteArray& content);
			/**
			 * Check if URL is tracked, and track if not.
			 */
			bool canRequestURL(const Swift::URL& url);
			void untrackURL(const Swift::URL& url);
			void putSkillLevelIntoRoot(boost::shared_ptr<SkillItem> root, boost::shared_ptr<SkillLevel> skillLevel);
			void loadSkillPlans(Character::ref character);
			void handleSkillPlanWantsToSave(const Swift::URL& characterURL, boost::shared_ptr<SkillPlanList> plans);
			void handleCharacterWantsUpdate(Character::ref character);
			void handleTimerTick();
			void handleRawError(const Swift::URL& url);
		private:
			Swift::NetworkFactories* factories_;
			boost::shared_ptr<DataStore> store_;
			std::map<std::string, std::string> accountKeys_;
			std::map<std::string, std::string> characterAccounts_;
			std::map<std::string /*key*/, Character::ref> characters_;
			std::set<std::string> requestsInFlight_;
			std::map<std::string /*characterID*/, bool> ignoredCharacters_;
			boost::shared_ptr<SkillTree> skillTree_;
			boost::shared_ptr<Swift::Timer> timer_;
			std::set<std::string> ignoredRequests_;
	};

}

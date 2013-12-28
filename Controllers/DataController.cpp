/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/DataController.h>

#include <vector>

#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Base/URL.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/DateTime.h>
#include <Swiften/Network/NetworkFactories.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

#include <Eve-Xin/Controllers/DataStore.h>
#include <Eve-Xin/Controllers/GeneralResult.h>
#include <Eve-Xin/Controllers/HTTPRequest.h>
#include <Eve-Xin/Controllers/SqliteDataStore.h>
#include <Eve-Xin/Controllers/Skill.h>
#include <Eve-Xin/Controllers/SkillGroup.h>
#include <Eve-Xin/Controllers/SkillLevel.h>
#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {

Swift::URL SKILL_URL("https","api.eveonline.com","/eve/SkillTree.xml.aspx");

DataController::DataController(Swift::NetworkFactories* factories) : factories_(factories) {
	Swift::PlatformApplicationPathProvider paths("Eve-Xin");
	store_ = boost::make_shared<SqliteDataStore>(paths.getDataDir());
	skillTree_ = boost::make_shared<SkillTree>();
	std::vector<DataStore::APIKey> keys = store_->getAPIKeys();
	foreach (const DataStore::APIKey& key, keys) {
		addAPIKey(key.key, key.ver, true);
	}
	getURLandDommify(SKILL_URL, boost::bind(&DataController::handleSkillResult, this, _1));
}

DataController::~DataController() {

}

void DataController::addAPIKey(const std::string& keyID, const std::string& vCode, bool isFromCache) {
	if (!isFromCache) {
		store_->addAPIKey(keyID, vCode);
	}
	accountKeys_[keyID] = vCode;
	Swift::URL url("https", "api.eveonline.com", "/account/APIKeyInfo.xml.aspx?keyID=" + keyID + "&vCode=" + vCode);
	ParsedCallback callback = boost::bind(&DataController::handleCharactersResult, this, keyID, _1);
	getURLandDommify(url, callback);
}

Swift::ByteArray DataController::getAndCache(const Swift::URL& url, RawCallback callback) {
	Swift::ByteArray content = store_->getContent(url);
	if (content.empty()) {
		if (canRequestURL(url)) {
			HTTPRequest::ref request = boost::make_shared<HTTPRequest>(url, factories_);
			//request->onError //FIXME: Do something with this
			request->onComplete.connect(boost::bind(&DataController::handleRawResult, this, url, _1, callback));
			request->send();
		}
	}
	else {
		if (!callback.empty()) {
			callback(content);
		}
	}
	return content;
}

void DataController::handleRawResult(const Swift::URL& url, const Swift::ByteArray& content, RawCallback callback) {
	untrackURL(url);
	store_->setContent(url, content);
	if (!callback.empty()) {
		callback(content);
	}
}

void DataController::getURLandDommify(const Swift::URL& url, ParsedCallback callback) {
	Swift::ByteArray content = store_->getContent(url);
	boost::shared_ptr<GeneralResult> result = boost::make_shared<GeneralResult>(content, factories_->getXMLParserFactory());
	if (!result->isValid() || result->needsRefresh()) {
		std::cerr << "Requesting update, isValid(" << result->isValid() << ") needsRefresh(" << result->needsRefresh() << ") cachedUntil(" << (result->isValid() ? Swift::dateTimeToLocalString(result->getCachedUntil()) : std::string("None")) << ")" << std::endl;
		if (canRequestURL(url)) {
			HTTPRequest::ref request = boost::make_shared<HTTPRequest>(url, factories_);
			//request->onError //FIXME: Do something with this
			request->onComplete.connect(boost::bind(&DataController::handleDOMResult, this, url, _1, callback));
			request->send();
		}
	}
	if (result->isValid() && !callback.empty()) {
		callback(result);
	}
}

void DataController::handleDOMResult(const Swift::URL& url, const Swift::ByteArray& content, ParsedCallback callback) {
	untrackURL(url);
	boost::shared_ptr<GeneralResult> result = boost::make_shared<GeneralResult>(content, factories_->getXMLParserFactory());
	if (result->isValid() && result->getResult()) {
		std::cerr << "Valid result" << std::endl;
		std::cerr << "Updating cache" << std::endl;
		store_->setContent(url, content);
		if (!callback.empty()) {
			callback(result);
		}
	}
	else {
		std::cerr << "Invalid result" << std::endl;
	}
}

void DataController::handleCharactersResult(const std::string& accountKey, boost::shared_ptr<GeneralResult> result) {
	std::cerr << "DataController<<handleCharactersResult:" << std::endl;
	Swift::ParserElement::ref keyElement = result->getResult()->getChild("key", "");
	const std::vector<Swift::ParserElement::ref>& characters = keyElement->getChild("rowset", "")->getChildren("row", "");
	std::string expires = keyElement->getAttributes().getAttribute("expires");
	std::cerr << "Account " << accountKey << " with " << characters.size() << " characters expires at " << expires << std::endl;
	foreach (auto element, characters) {
		std::string id = element->getAttributes().getAttribute("characterID");
		std::string name = element->getAttributes().getAttribute("characterName");
		std::string corpKey = element->getAttributes().getAttribute("corporationID");
		std::string corpName = element->getAttributes().getAttribute("corporationName");
		Character::ref oldCharacter = characters_[id];
		Character::ref character = boost::make_shared<Character>(id, name, accountKey, corpKey, corpName, expires);
		characters_[id] = character;
		characterAccounts_[id] = accountKey;
		getCharacter(id);
		if (!oldCharacter) {
			//These data won't be changing, so don't issue changes if we already had them
			onCharacterDataChanged(id);
		}
	}
	
}

void DataController::handleAccountBalanceResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result) {
	Swift::ParserElement::ref element = result->getResult()->getChild("rowset", "")->getChild("row", "");
	std::string balance = element->getAttributes().getAttribute("balance");
	std::cerr << "Balance for " << characterID << ": " << balance << std::endl;
	Character::ref character = characters_[characterID];
	if (!character) {
		return;
	}
	double oldISK = character->getISK();
	character->setISK(boost::lexical_cast<double>(balance));
	if (oldISK != character->getISK()) {
		onCharacterDataChanged(characterID);
	}
}

void DataController::handleSkillResult(boost::shared_ptr<GeneralResult> result) {
	// http://wiki.eve-id.net/APIv2_Eve_SkillTree_XMLs
	// https://wiki.eveonline.com/en/wiki/EVE_API_EVE_Skill_Tree
	const std::vector<Swift::ParserElement::ref>& groups = result->getResult()->getChild("rowset", "")->getChildren("row", "");
	foreach (Swift::ParserElement::ref groupElement, groups) {
		std::string groupID = groupElement->getAttributes().getAttribute("groupID");
		std::string groupName = groupElement->getAttributes().getAttribute("groupName");
		SkillGroup::ref group = boost::make_shared<SkillGroup>(groupID, groupName);
		skillTree_->addGroup(group);
		const std::vector<Swift::ParserElement::ref>& skills = groupElement->getChild("rowset", "")->getChildren("row", "");
		foreach (Swift::ParserElement::ref skillElement, skills) {
			std::string skillID = skillElement->getAttributes().getAttribute("typeID");
			std::string skillName = skillElement->getAttributes().getAttribute("typeName");
			std::string description = skillElement->getChild("description", "")->getText();
			int rank = 0;
			try {
				rank = boost::lexical_cast<int>(skillElement->getChild("rank", "")->getText());
			}
			catch(const boost::bad_lexical_cast &) {
				//Not much to do if they send bad data
			}
			Swift::ParserElement::ref attributesElement = skillElement->getChild("requiredAttributes", "");
			std::string primaryAttribute = attributesElement->getChild("primaryAttribute", "")->getText();
			std::string secondaryAttribute = attributesElement->getChild("secondaryAttribute", "")->getText();
			const std::vector<Swift::ParserElement::ref>& rowsets = skillElement->getChildren("rowset", "");
			std::vector<SkillLevel> dependencies;
			foreach (Swift::ParserElement::ref row, rowsets) {
				if (row->getAttributes().getAttribute("name") == "requiredSkills") {
					const std::vector<Swift::ParserElement::ref>& dependencyElements = skillElement->getChildren("row", "");
					foreach (Swift::ParserElement::ref dependencyElement, dependencyElements) {
						std::string dependencyID = dependencyElement->getAttributes().getAttribute("typeID");
						int dependencyLevel = 0;
						try {
							dependencyLevel = boost::lexical_cast<int>(dependencyElement->getAttributes().getAttribute("skillLevel"));
						}
						catch(const boost::bad_lexical_cast &) {
							//Not much to do if they send bad data
						}
						dependencies.push_back(SkillLevel(skillTree_->getSkill(dependencyID), dependencyLevel));
					}
				}
			}
			
			Skill::ref skill = skillTree_->getSkill(skillID);
			skill->populate(skillID, groupID, skillName, description, rank, primaryAttribute, secondaryAttribute, dependencies);
			group->addChild(skill);
			skill->setGroupRef(group);
		}
	}
}

void DataController::putSkillLevelIntoRoot(boost::shared_ptr<SkillGroup> root, boost::shared_ptr<SkillLevel> skillLevel) {
	const std::vector<SkillItem::ref>& children = root->getChildren();
	bool found = false;
	SkillGroup::ref group;
	std::string groupID = skillLevel->getSkill()->getGroupID();
	foreach (SkillItem::ref child, children) {
		group = boost::dynamic_pointer_cast<SkillGroup>(child);
		if (!group) continue;
		if (group->getID() == groupID) {
			found = true;
			break;
		}
	}
	if (!found) {
		SkillGroup::ref realGroup = skillTree_->getGroup(groupID);
		group = boost::make_shared<SkillGroup>(realGroup->getID(), realGroup->getName());
		root->addChild(group);
	}
	group->addChild(skillLevel);
}

void DataController::handleCharacterSheetResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result) {
	Character::ref character = characters_[characterID];
	if (!character) {
		return;
	}
	// http://wiki.eve-id.net/APIv2_Char_CharacterSheet_XML
	std::string race;
	std::string bloodline;
	std::string gender;
	std::string allianceName;
	std::string allianceID;
	std::string cloneName;
	std::string cloneSkillPoints;
	std::string balance; // Only use this if not yet populated, as the account request will pretty much always be newer
	std::string memoryBonusName;
	int memoryBonusValue;
	int intelligence;
	int memory;
	int charisma;
	int perception;
	int willpower;
	//skills
	const std::vector<Swift::ParserElement::ref>& rowsets = result->getResult()->getChildren("rowset", "");
	foreach (Swift::ParserElement::ref rowset, rowsets) {
		if (rowset->getAttributes().getAttribute("name") == "skills") {
			SkillGroup::ref skillRoot = boost::make_shared<SkillGroup>();
			character->setKnownSkills(skillRoot);
			const std::vector<Swift::ParserElement::ref>& rows = result->getResult()->getChildren("row", "");
			foreach (Swift::ParserElement::ref row, rows) {
				std::string skillID = row->getAttributes().getAttribute("typeID");
				int level = 0;
				try {
					level = boost::lexical_cast<int>(row->getAttributes().getAttribute("level"));
				}
				catch(const boost::bad_lexical_cast &) {
					//Not much to do if they send bad data
				}
				SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(skillTree_->getSkill(skillID), level);
				//FIXME: Put in skill points too
				putSkillLevelIntoRoot(skillRoot, skillLevel);
			}
		}
	}
	//certificates
	//corporationRoles
	//corporationRolesAtHQ
	//corporationTitles

	//onCharacterDataChanged(characterID); // online if it changed
}

Character::ref DataController::getCharacter(const std::string& id) {
	std::string accountKey = characterAccounts_[id];
	std::string vCode = accountKeys_[accountKey];
	Swift::URL bigAvatarURL("http", "image.eveonline.com", "/Character/" + id + "_256.jpg");
	getAndCache(bigAvatarURL, boost::bind(&DataController::handleCharacter256AvatarResult, this, id, _1));
	Swift::URL characterSheetURL("https", "api.eveonline.com", "/char/CharacterSheet.xml.aspx?keyID=" + accountKey + "&vCode=" + vCode + "&characterID=" + id);
	getURLandDommify(characterSheetURL, boost::bind(&DataController::handleCharacterSheetResult, this, id, _1));
	Swift::URL accountBalanceURL("https", "api.eveonline.com", "/char/AccountBalance.xml.aspx?keyID=" + accountKey + "&vCode=" + vCode + "&characterID=" + id);
	getURLandDommify(accountBalanceURL, boost::bind(&DataController::handleAccountBalanceResult, this, id, _1));
	return characters_[id];
}

void DataController::handleCharacter256AvatarResult(const std::string& id, const Swift::ByteArray& content) {
	Character::ref character = characters_[id];
	if (!character) {
		return;
	}
	character->set256Avatar(content);
}

std::vector<std::string> DataController::getCharacters() {
	std::vector<std::string> characters;
	foreach (auto i, characters_) {
		characters.push_back(i.first);
	}
	return characters;
}

Swift::ByteArray DataController::get256CharacterAvatar(const std::string& id) {
	//http://wiki.eve-id.net/APIv2_Eve_Image_Service
	Swift::URL bigAvatarURL("https", "image.eveonline.com", "/Character/" + id + "_256.jpg");
	return getAndCache(bigAvatarURL, NULL);
}

bool DataController::canRequestURL(const Swift::URL& url) {
	if (requestsInFlight_.count(url.toString()) > 0) {
		return false;
	}
	requestsInFlight_.insert(url.toString());
	return true;
}

void DataController::untrackURL(const Swift::URL& url) {
	requestsInFlight_.erase(url.toString());
}

}

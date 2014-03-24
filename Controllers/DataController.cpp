/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/DataController.h>

#include <vector>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/URL.h>
#include <Swiften/Network/NetworkFactories.h>

#include <Eve-Xin/Controllers/DataStore.h>
#include <Eve-Xin/Controllers/GeneralResult.h>
#include <Eve-Xin/Controllers/HTTPRequest.h>
#include <Eve-Xin/Controllers/SqliteDataStore.h>
#include <Eve-Xin/Controllers/Skill.h>
#include <Eve-Xin/Controllers/SkillAttribute.h>
#include <Eve-Xin/Controllers/SkillItem.h>
#include <Eve-Xin/Controllers/SkillLevel.h>
#include <Eve-Xin/Controllers/SkillOrderedList.h>
#include <Eve-Xin/Controllers/SkillPlan.h>
#include <Eve-Xin/Controllers/SkillPlanList.h>
#include <Eve-Xin/Controllers/SkillPlanSerialization.h>
#include <Eve-Xin/Controllers/SkillTree.h>

namespace EveXin {

Swift::URL SKILL_URL("https","api.eveonline.com","/eve/SkillTree.xml.aspx");

DataController::DataController(Swift::NetworkFactories* factories, const boost::filesystem::path& dataDir) : factories_(factories) {
	store_ = boost::make_shared<SqliteDataStore>(dataDir);
	skillTree_ = boost::make_shared<SkillTree>();
	getURLandDommifySince(SKILL_URL, Swift::stringToDateTime("2014-03-23T11:00:00Z"), boost::bind(&DataController::handleSkillResult, this, _1));
	std::vector<DataStore::APIKey> keys = store_->getAPIKeys();
	foreach (const DataStore::APIKey& key, keys) {
		addAPIKey(key.key, key.ver, true);
	}
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

void DataController::deleteAPIKey(const std::string& keyID) {
	accountKeys_.erase(keyID);
	store_->deleteAPIKey(keyID);
}

std::vector<std::string> DataController::getAPIKeys() {
	std::vector<std::string> keys;
	foreach (auto keyPair, accountKeys_) {
		keys.push_back(keyPair.first);
	}
	return keys;
}

std::vector<Character::ref> DataController::getCharactersForAPIKey(const std::string& apiKey) {
	std::vector<Character::ref> characters;
	foreach (auto pair, characterAccounts_) {
		if (pair.second == apiKey) {
			characters.push_back(characters_[pair.first]);
		}
	}
	return characters;
}

void DataController::setCharacterIgnored(const std::string& characterID, bool ignored) {
	ignoredCharacters_[characterID] = ignored;
}

Swift::ByteArray DataController::getAndCache(const Swift::URL& url, RawCallback callback) {
	Swift::ByteArray content = store_->getContent(url);
	// We're expecting a file here, not a normal HTTP response. So if it's not a file, assume it's wrong
	if (Swift::byteArrayToString(content).substr(0, 4) == "HTTP") {
		content.clear();
	}
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

void DataController::getURLandDommifySince(const Swift::URL& url, boost::posix_time::ptime since, ParsedCallback callback) {
	Swift::ByteArray content = store_->getContent(url);
	boost::shared_ptr<GeneralResult> result = boost::make_shared<GeneralResult>(content, factories_->getXMLParserFactory());
	if (!result->isValid() || result->needsRefresh() || since > result->getDate()) {
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

void DataController::getURLandDommify(const Swift::URL& url, ParsedCallback callback) {
	getURLandDommifySince(url, Swift::stringToDateTime("1984-01-01T00:00:00Z"), callback);
}

void DataController::handleDOMResult(const Swift::URL& url, const Swift::ByteArray& content, ParsedCallback callback) {
	untrackURL(url);
	boost::shared_ptr<GeneralResult> result = boost::make_shared<GeneralResult>(content, factories_->getXMLParserFactory());
	if (result->isValid() && result->getResult()) {
		store_->setContent(url, content);
		if (!callback.empty()) {
			callback(result);
		}
	}
	else {
		// std::cerr << "Invalid result" << std::endl;
	}
}

void DataController::handleCharactersResult(const std::string& accountKey, boost::shared_ptr<GeneralResult> result) {
	Swift::ParserElement::ref keyElement = result->getResult()->getChild("key", "");
	const std::vector<Swift::ParserElement::ref>& characters = keyElement->getChild("rowset", "")->getChildren("row", "");
	std::string expires = keyElement->getAttributes().getAttribute("expires");
	size_t oldSize = characters_.size();
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
	}
	if (oldSize != characters_.size()) {
		onCharacterListChanged();
	}
	
}

void DataController::handleAccountBalanceResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result) {
	Swift::ParserElement::ref element = result->getResult()->getChild("rowset", "")->getChild("row", "");
	std::string balance = element->getAttributes().getAttribute("balance");
	Character::ref character = characters_[characterID];
	if (!character) {
		return;
	}
	character->setISK(boost::lexical_cast<double>(balance));
}

void DataController::handleSkillResult(boost::shared_ptr<GeneralResult> result) {
	// http://wiki.eve-id.net/APIv2_Eve_SkillTree_XMLs
	// https://wiki.eveonline.com/en/wiki/EVE_API_EVE_Skill_Tree
	const std::vector<Swift::ParserElement::ref>& groups = result->getResult()->getChild("rowset", "")->getChildren("row", "");
	foreach (Swift::ParserElement::ref groupElement, groups) {
		std::string groupID = groupElement->getAttributes().getAttribute("groupID");
		std::string groupName = groupElement->getAttributes().getAttribute("groupName");
		SkillItem::ref group = skillTree_->getChild(groupID);

		const std::vector<Swift::ParserElement::ref>& skills = groupElement->getChild("rowset", "")->getChildren("row", "");
		foreach (Swift::ParserElement::ref skillElement, skills) {
			std::string skillID = skillElement->getAttributes().getAttribute("typeID");
			std::string skillName = skillElement->getAttributes().getAttribute("typeName");
			std::string description = skillElement->getChild("description", "")->getText();
			if (skillElement->getAttributes().getAttribute("published") == "0") {
				continue;
			}
			if (!group) {
				group = boost::make_shared<SkillItem>(skillTree_, groupID, groupName);
				skillTree_->addChild(group);
			}
			int rank = 0;
			try {
				rank = boost::lexical_cast<int>(skillElement->getChild("rank", "")->getText());
			}
			catch(const boost::bad_lexical_cast &) {
				//Not much to do if they send bad data
			}
			Swift::ParserElement::ref attributesElement = skillElement->getChild("requiredAttributes", "");
			SkillAttribute::Attribute primaryAttribute = SkillAttribute::stringToAttribute( attributesElement->getChild("primaryAttribute", "")->getText());
			SkillAttribute::Attribute secondaryAttribute = SkillAttribute::stringToAttribute( attributesElement->getChild("secondaryAttribute", "")->getText());
			const std::vector<Swift::ParserElement::ref>& rowsets = skillElement->getChildren("rowset", "");
			std::vector<SkillLevel::ref> dependencies;
			Skill::ref skill = skillTree_->getSkill(skillID);
			SkillItem::ref skillItem = boost::make_shared<SkillItem>(group, skill);
			group->addChild(skillItem);
			foreach (Swift::ParserElement::ref row, rowsets) {
				if (row->getAttributes().getAttribute("name") == "requiredSkills") {
					const std::vector<Swift::ParserElement::ref>& dependencyElements = row->getChildren("row", "");
					foreach (Swift::ParserElement::ref dependencyElement, dependencyElements) {
						std::string dependencyID = dependencyElement->getAttributes().getAttribute("typeID");
						int dependencyLevel = 0;
						try {
							dependencyLevel = boost::lexical_cast<int>(dependencyElement->getAttributes().getAttribute("skillLevel"));
						}
						catch(const boost::bad_lexical_cast &) {
							//Not much to do if they send bad data
						}
						SkillLevel::ref dependency = boost::make_shared<SkillLevel>(skillItem, skillTree_->getSkill(dependencyID), dependencyLevel);
						skillItem->addChild(dependency);
						dependencies.push_back(dependency);
					}
				}
			}
			
			
			skill->populate(groupID, group, skillName, description, rank, primaryAttribute, secondaryAttribute, dependencies);
		}
	}
	onSkillTreeChanged();
}

void DataController::handleSkillQueueResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result) {
	Character::ref character = characters_[characterID];
	if (!character) {
		std::cerr << "Receiving skill queue for unexpected ID" << characterID << std::endl;
		return;
	}
	const std::vector<Swift::ParserElement::ref>& rows = result->getResult()->getChild("rowset", "")->getChildren("row", "");
	std::vector<Swift::ParserElement::ref> orderedRows;
	foreach (Swift::ParserElement::ref row, rows) {
		// https://wiki.eveonline.com/en/wiki/EVE_API_Character_Skill_Queue
		// doesn't say these are in order, so I guess I'd better re-order to be on the safe side
		try {
			size_t i = boost::lexical_cast<size_t>(row->getAttributes().getAttribute("queuePosition"));
			while (orderedRows.size() < i + 1) {
				orderedRows.push_back(Swift::ParserElement::ref());
			}
			orderedRows[i] = row;
		} catch (const boost::bad_lexical_cast&) {
			std::cerr << "skill queue position cast failed of " << row->getAttributes().getAttribute("queuePosition") << std::endl;
		}
	}
	SkillItem::ref root = boost::make_shared<SkillItem>(SkillItem::ref(), "queueid", "Queue Root");
	SkillItem::ref group = boost::make_shared<SkillOrderedList>(root, "queuegroupid", "Skill Training Queue");
	root->addChild(group);

	foreach (Swift::ParserElement::ref row, orderedRows) {
		std::string skillID = row->getAttributes().getAttribute("typeID");
		int level = 0;
		try {
			level = boost::lexical_cast<int>(row->getAttributes().getAttribute("level"));
		}
		catch(const boost::bad_lexical_cast &) {
			std::cerr << "skill queue level cast failed" << std::endl;
		}
		int startPoints = 0;
		try {
			startPoints = boost::lexical_cast<int>(row->getAttributes().getAttribute("startSP"));
		}
		catch(const boost::bad_lexical_cast &) {
			std::cerr << "skill queue start points cast failed" << std::endl;
		}
		int endPoints = 0;
		try {
			endPoints = boost::lexical_cast<int>(row->getAttributes().getAttribute("endSP"));
		}
		catch(const boost::bad_lexical_cast &) {
			std::cerr << "skill queue start points cast failed" << std::endl;
		}
		std::string startTimeString = row->getAttributes().getAttribute("startTime");
		std::string endTimeString = row->getAttributes().getAttribute("endTime");
		boost::posix_time::ptime startTime = Swift::stringToDateTime(startTimeString);
		boost::posix_time::ptime endTime = Swift::stringToDateTime(endTimeString);
		Skill::ref skill = skillTree_->getSkill(skillID);
		SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(group, skill, level, startPoints, startTime, endTime);
		group->addChild(skillLevel);
	}
	character->setTrainingQueue(root);
}

void DataController::handleCharacterSheetResult(const std::string& characterID, boost::shared_ptr<GeneralResult> result) {
	Character::ref character = characters_[characterID];
	if (!character) {
		std::cerr << "Receiving character sheet for unexpected ID" << characterID << std::endl;
		return;
	}
	//std::cerr << "Handling character " << characterID << "'s sheet." << std::endl;
	// http://wiki.eve-id.net/APIv2_Char_CharacterSheet_XML
	std::string race;
	std::string bloodline;
	std::string gender;
	std::string allianceName;
	std::string allianceID;
	std::string cloneName;
	std::string cloneSkillPoints;
	std::string balance; // Only use this if not yet populated, as the account request will pretty much always be newer
	Swift::ParserElement::ref attributeEnhancers = result->getResult()->getChild("attributeEnhancers", "");
	try {
		std::map<std::string, SkillAttribute::Attribute> tags;
		tags["intelligenceBonus"] = SkillAttribute::Intelligence;
		tags["memoryBonus"] = SkillAttribute::Memory;
		tags["charismaBonus"] = SkillAttribute::Charisma;
		tags["perceptionBonus"] = SkillAttribute::Perception;
		tags["willpowerBonus"] = SkillAttribute::Willpower;
		std::map<SkillAttribute::Attribute, int> enhancerValues;
		std::map<SkillAttribute::Attribute, std::string> enhancerNames;
		foreach (auto pair, tags) {
			Swift::ParserElement::ref attributeEnhancer = attributeEnhancers->getChild(pair.first, "");
			std::string name = attributeEnhancer->getChild("augmentatorName", "")->getText();
			std::string value = attributeEnhancer->getChild("augmentatorValue", "")->getText();
			if (!value.empty()) {
				enhancerValues[pair.second] = boost::lexical_cast<int>(value);
				enhancerNames[pair.second] = name;
			}
		}
		character->setImplants(enhancerNames, enhancerValues);
	} catch (const boost::bad_lexical_cast&) {
		std::cerr << "cast failed" << std::endl;
	}
	try {
		character->setAttribute(SkillAttribute::Intelligence, boost::lexical_cast<int>(result->getResult()->getChild("attributes", "")->getChild("intelligence", "")->getText()));
		character->setAttribute(SkillAttribute::Memory, boost::lexical_cast<int>(result->getResult()->getChild("attributes", "")->getChild("memory", "")->getText()));
		character->setAttribute(SkillAttribute::Charisma, boost::lexical_cast<int>(result->getResult()->getChild("attributes", "")->getChild("charisma", "")->getText()));
		character->setAttribute(SkillAttribute::Perception, boost::lexical_cast<int>(result->getResult()->getChild("attributes", "")->getChild("perception", "")->getText()));
		character->setAttribute(SkillAttribute::Willpower, boost::lexical_cast<int>(result->getResult()->getChild("attributes", "")->getChild("willpower", "")->getText()));
	} catch (const boost::bad_lexical_cast&) {	
	}
	//skills
	const std::vector<Swift::ParserElement::ref>& rowsets = result->getResult()->getChildren("rowset", "");
	foreach (Swift::ParserElement::ref rowset, rowsets) {
		if (rowset->getAttributes().getAttribute("name") == "skills") {
			SkillItem::ref skillRoot = boost::make_shared<SkillItem>(SkillItem::ref(), "char_root", "char_root");
			const std::vector<Swift::ParserElement::ref>& rows = rowset->getChildren("row", "");
			foreach (Swift::ParserElement::ref row, rows) {
				if (row->getAttributes().getAttribute("published") == "0") {
					continue;
				}
				std::string skillID = row->getAttributes().getAttribute("typeID");
				int level = 0;
				try {
					level = boost::lexical_cast<int>(row->getAttributes().getAttribute("level"));
				}
				catch(const boost::bad_lexical_cast &) {
					//Not much to do if they send bad data
				}
				int points = 0;
				try {
					points = boost::lexical_cast<int>(row->getAttributes().getAttribute("skillpoints"));
				}
				catch(const boost::bad_lexical_cast &) {
					//Not much to do if they send bad data
				}
				Skill::ref skill = skillTree_->getSkill(skillID);
				// There is an unlikely race condition here, where a character sheet might get returned before
				// the full skill set has been fetched. In that case the skillTree_ won't be fully populated, 
				// so cope with that and insert dummy groups in the character tree until the next refresh.
				std::string groupID = skill->getGroupID();
				SkillItem::ref parent = skillTree_->getGroup(groupID);
				std::string groupName = parent ? parent->getName() : "Please refresh";
				SkillItem::ref group = skillRoot->getGroup(groupID, groupName);
				SkillLevel::ref skillLevel = boost::make_shared<SkillLevel>(group, skill, level, points);
				group->addChild(skillLevel);
			}
			character->setKnownSkills(skillRoot);
		}
	}
	//certificates
	//corporationRoles
	//corporationRolesAtHQ
	//corporationTitles
	//
	loadSkillPlans(character);

	//onCharacterDataChanged(characterID); // online if it changed
}

void DataController::loadSkillPlans(Character::ref character) {
	SkillPlanList::ref planRoot;
	Swift::URL url = Swift::URL::fromString("http://characters/" + character->getID());
	planRoot = SkillPlanSerialization::parseSkills(store_->getContent(url), skillTree_, character->getKnownSkills());

	planRoot->onWantsToSave.connect(boost::bind(&DataController::handleSkillPlanWantsToSave, this, url, planRoot));

	if (planRoot->getChildren().empty()) {
		SkillPlan::ref plan1 = planRoot->createPlan("Default Plan");
		plan1->disableSaving();
		plan1->enableSaving();
	}
	character->setSkillPlanRoot(planRoot);
}

Character::ref DataController::getCharacter(const std::string& id) {
	std::string accountKey = characterAccounts_[id];
	std::string vCode = accountKeys_[accountKey];
	std::vector<size_t> avatarSizes;
	avatarSizes.push_back(32);
	avatarSizes.push_back(256);
	foreach (size_t avatarSize, avatarSizes) {
		Swift::URL avatarURL("http", "image.eveonline.com", "/Character/" + id + "_" + boost::lexical_cast<std::string>(avatarSize) + ".jpg");
		getAndCache(avatarURL, boost::bind(&DataController::handleCharacterAvatarResult, this, id, avatarSize, _1));
	}
	Swift::URL characterSheetURL("https", "api.eveonline.com", "/char/CharacterSheet.xml.aspx?keyID=" + accountKey + "&vCode=" + vCode + "&characterID=" + id);
	getURLandDommify(characterSheetURL, boost::bind(&DataController::handleCharacterSheetResult, this, id, _1));
	Swift::URL accountBalanceURL("https", "api.eveonline.com", "/char/AccountBalance.xml.aspx?keyID=" + accountKey + "&vCode=" + vCode + "&characterID=" + id);
	getURLandDommify(accountBalanceURL, boost::bind(&DataController::handleAccountBalanceResult, this, id, _1));
	Swift::URL skillQueueURL("https", "api.eveonline.com", "/char/SkillQueue.xml.aspx?keyID=" + accountKey + "&vCode=" + vCode + "&characterID=" + id);
	getURLandDommify(skillQueueURL, boost::bind(&DataController::handleSkillQueueResult, this, id, _1));

	return characters_[id];
}

void DataController::handleCharacterAvatarResult(const std::string& id, size_t size, const Swift::ByteArray& content) {
	Character::ref character = characters_[id];
	if (!character) {
		return;
	}
	character->setAvatar(size, content);
}

std::vector<std::string> DataController::getCharacters() {
	std::vector<std::string> characters;
	foreach (auto i, characters_) {
		if (!ignoredCharacters_[i.first]) {
			characters.push_back(i.first);
		}
	}
	return characters;
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

boost::shared_ptr<SkillTree> DataController::getSkillTree() {
	return skillTree_;
}

void DataController::handleSkillPlanWantsToSave(const Swift::URL& characterURL, SkillPlanList::ref planList) {
	store_->setContent(characterURL, SkillPlanSerialization::serializeSkills(planList));
}

}

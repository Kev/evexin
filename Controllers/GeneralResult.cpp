/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/GeneralResult.h>

#include <deque>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Parser/XMLParser.h>
#include <Swiften/Parser/XMLParserClient.h>
#include <Swiften/Parser/XMLParserFactory.h>

namespace EveXin {

class Dommifier : public Swift::XMLParserClient {
	public:
		virtual void handleStartElement(const std::string& element, const std::string& xmlns, const Swift::AttributeMap& attributes) {
			if (!root_) {
				root_ = boost::make_shared<Swift::ParserElement>(element, xmlns, attributes);
				elementStack_.push_back(root_);
			}
			else {
				Swift::ParserElement::ref current = *elementStack_.rbegin();
				elementStack_.push_back(current->addChild(element, xmlns, attributes));
			}
		}

		virtual void handleEndElement(const std::string& /*element*/, const std::string&) {
			elementStack_.pop_back();
		}

		virtual void handleCharacterData(const std::string& data) {
			Swift::ParserElement::ref current = *elementStack_.rbegin();
			current->appendCharacterData(data);
		}

		Swift::ParserElement::ref getRoot() {
			return root_;
		}

	private:
		std::deque<Swift::ParserElement::ref> elementStack_;
		Swift::ParserElement::ref root_;
};

GeneralResult::GeneralResult(const Swift::ByteArray& source, Swift::XMLParserFactory* parserFactory) : valid_(true) {
	if (source.empty()) {
		valid_ = false;
		return;
	}
	Dommifier dommifier;
	boost::shared_ptr<Swift::XMLParser> parser(parserFactory->createXMLParser(&dommifier));
	valid_ = parser->parse(Swift::byteArrayToString(source));
	if (!valid_) {
		return;
	}
	Swift::ParserElement::ref root = dommifier.getRoot();
	std::string cachedUntilString = root->getChild("cachedUntil", "")->getText();
	std::cerr << "Cached until " << cachedUntilString << std::endl;
	cachedUntil_ = Swift::stringToDateTime(cachedUntilString + "Z");
	std::string deliveredAtString = root->getChild("currentTime", "")->getText();
	std::cerr << "Delivered at " << deliveredAtString << std::endl;
	date_ = Swift::stringToDateTime(deliveredAtString + "Z");

	//Some things come through with a dodgy cachedUntil of immediate when they should be an hour, so if they're the same, add an hour.
	if (date_ == cachedUntil_) {
		cachedUntil_ = cachedUntil_ + boost::posix_time::hours(1);
	}

	result_ = root->getChild("result", "");
}

GeneralResult::~GeneralResult() {
	
}

boost::posix_time::ptime GeneralResult::getCachedUntil() {
	return cachedUntil_;
}

boost::posix_time::ptime GeneralResult::getDate() {
	return date_;
}

Swift::ParserElement::ref GeneralResult::getResult() {
	return result_;
}

bool GeneralResult::isValid() {
	return valid_;
}

bool GeneralResult::needsRefresh() {
	return getCachedUntil() < boost::posix_time::second_clock::universal_time();
}

}

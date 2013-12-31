/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/SqliteDataStore.h>

#include <sqlite3.h>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Path.h>
#include <Swiften/StringCodecs/Base64.h>

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace EveXin {

SqliteDataStore::SqliteDataStore(boost::filesystem::path dataDir) : dataDir_(dataDir) {
	boost::filesystem::path boostPath = dataDir / "db.sqlite3";
	std::string stringPath = Swift::pathToString(boostPath);
	const char * sqlitePath = stringPath.c_str(); // Just to play with for the moment
	int openResult = sqlite3_open(sqlitePath, &db_);
	if (openResult == SQLITE_OK) {
		int execResult = sqlite3_exec(db_, "PRAGMA encoding = 'UTF-8';", NULL, NULL, NULL);
		if (execResult != SQLITE_OK) {
			std::cerr << "Couldn't set UTF-8 for db_ " << sqlitePath << ", " << sqlite3_errmsg(db_) << std::endl;
			sqlite3_close(db_);
			db_ = NULL;
			return;
		}
		execResult = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS apiCache('url' text, 'content' text);", NULL, NULL, NULL);
		if (execResult != SQLITE_OK) {
			std::cerr << "Couldn't create db_ " << sqlitePath << ", " << sqlite3_errmsg(db_) << std::endl;
			sqlite3_close(db_);
			db_ = NULL;
			return;

		}
		execResult = sqlite3_exec(db_, "CREATE TABLE IF NOT EXISTS apiKeys('key' text, 'ver' text);", NULL, NULL, NULL);
		if (execResult != SQLITE_OK) {
			std::cerr << "Couldn't create db_ " << sqlitePath << ", " << sqlite3_errmsg(db_) << std::endl;
			sqlite3_close(db_);
			db_ = NULL;
			return;
		}
		std::cerr << " DB_ setup " << sqlitePath << std::endl;
	}
	else {
		std::cerr << "ERROR:  Couldn't open sqlite database " << sqlitePath << ", " << openResult << ", " << sqlite3_errmsg(db_) << std::endl;
		db_ = NULL;
	}
}

SqliteDataStore::~SqliteDataStore() {
	sqlite3_close(db_);
}

Swift::ByteArray SqliteDataStore::getContent(const Swift::URL& url) {
	std::string key = urlToKey(url);
	Swift::ByteArray content;

	sqlite3_stmt* preparedStatement = NULL;
	sqlite3_prepare_v2(db_, ("SELECT content FROM apiCache WHERE url='" + key + "'").c_str(), -1, &preparedStatement, NULL); // http://www.sqlite.org/c3ref/prepare.html
	if (sqlite3_step(preparedStatement) != SQLITE_ROW) {
		//std::cerr << "Failed to fetch content" << std::endl;
		sqlite3_finalize(preparedStatement);

		return content;
	}
	content = Swift::Base64::decode(getStringColumn(preparedStatement, 0));
	sqlite3_finalize(preparedStatement);
	//std::cerr << "sqlite loaded content " << Swift::byteArrayToString(content) << std::endl;
	return content;
}

void SqliteDataStore::setContent(const Swift::URL& url, const Swift::ByteArray& content) {
	std::string key = urlToKey(url);
	std::string stringContent = Swift::Base64::encode(content);
	const char* charContent = stringContent.c_str();
	sqlite3_stmt* preparedStatement = NULL;
	std::string command = "UPDATE apiCache SET content=? WHERE url='" + key + "'";
	//std::cerr << "Trying: " << command << std::endl;
	sqlite3_prepare_v2(db_, command.c_str(), -1, &preparedStatement, NULL);
	sqlite3_bind_text(preparedStatement, 1, charContent, -1, NULL);
	sqlite3_step(preparedStatement);
	sqlite3_finalize(preparedStatement);
	if (sqlite3_changes(db_) == 0) {
		preparedStatement = NULL;
		command = "INSERT INTO apiCache (content, url) VALUES (?, '" + key + "')";
		//std::cerr << "No update, so trying: " << command << std::endl;
		sqlite3_prepare_v2(db_, command.c_str(), -1, &preparedStatement, NULL);
		sqlite3_bind_text(preparedStatement, 1, charContent, -1, NULL);
		sqlite3_step(preparedStatement);
		sqlite3_finalize(preparedStatement);
		if (sqlite3_changes(db_) == 0) {
			std::cerr << "Error, error, nothing changed: " << sqlite3_errmsg(db_) << std::endl;
		}
	}
}

std::string SqliteDataStore::urlToKey(const Swift::URL& url) {
	std::string encoded = Swift::Base64::encode(Swift::createByteArray(url.toString()));
	return encoded;
}

void SqliteDataStore::addAPIKey(const std::string& key, const std::string& ver) {
	sqlite3_stmt* preparedStatement = NULL;
	std::string command = "INSERT INTO apiKeys (key, ver) VALUES (?, ?)";
	//std::cerr << "Trying: " << command << std::endl;
	sqlite3_prepare_v2(db_, command.c_str(), -1, &preparedStatement, NULL);
	sqlite3_bind_text(preparedStatement, 1, key.c_str(), -1, NULL);
	sqlite3_bind_text(preparedStatement, 2, ver.c_str(), -1, NULL);
	sqlite3_step(preparedStatement);
	sqlite3_finalize(preparedStatement);
	if (sqlite3_changes(db_) == 0) {
		std::cerr << "Error, error, nothing changed: " << sqlite3_errmsg(db_) << std::endl;
	}
}

std::vector<DataStore::APIKey> SqliteDataStore::getAPIKeys() {
	std::vector<APIKey> keys;
	sqlite3_stmt* preparedStatement = NULL;
	std::string command = "SELECT * FROM apiKeys";
	//std::cerr << "Trying: " << command << std::endl;
	sqlite3_prepare_v2(db_, command.c_str(), -1, &preparedStatement, NULL);
	int result = sqlite3_step(preparedStatement);
	while (result == SQLITE_ROW) {
		APIKey key;
		key.key = getStringColumn(preparedStatement, 0);
		key.ver = getStringColumn(preparedStatement, 1);
		//std::cerr << "Found key " << key.key << ", ver " << key.ver << std::endl;
		result = sqlite3_step(preparedStatement);
		keys.push_back(key);
	}
	sqlite3_finalize(preparedStatement);
	std::cerr << "Found " << keys.size() << " API Keys" << std::endl;
	return keys;
}

std::string SqliteDataStore::getStringColumn(sqlite3_stmt* statement, int column) {
	const char* charString = reinterpret_cast<const char*>(sqlite3_column_text(statement, column));
	return std::string(charString);
}

}

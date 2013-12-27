/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <boost/filesystem.hpp>

#include <Eve-Xin/Controllers/DataStore.h>

struct sqlite3;
struct sqlite3_stmt;

namespace Swift {
	class ApplicationPathProvider;
}

namespace EveXin {
	class SqliteDataStore : public DataStore {
		public:
			SqliteDataStore(boost::filesystem::path dataDir);
			virtual ~SqliteDataStore();

			virtual Swift::ByteArray getContent(const Swift::URL& url);
			virtual void setContent(const Swift::URL& url, const Swift::ByteArray& content);

			virtual void addAPIKey(const std::string& key, const std::string& ver);
			virtual std::vector<APIKey> getAPIKeys();
		private:
			std::string urlToKey(const Swift::URL& url);
			std::string getStringColumn(sqlite3_stmt* statement, int column);
		private:
			boost::filesystem::path dataDir_;
			sqlite3* db_;
	};

}

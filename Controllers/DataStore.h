/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/URL.h> 

namespace EveXin {
	class DataStore {
		public:
			struct APIKey {
				std::string key;
				std::string ver;
			};
			DataStore();
			virtual ~DataStore();

			virtual Swift::ByteArray getContent(const Swift::URL& filename) = 0;
			virtual void setContent(const Swift::URL& id, const Swift::ByteArray& content) = 0;

			virtual void addAPIKey(const std::string& key, const std::string& ver) = 0;
			virtual std::vector<APIKey> getAPIKeys() = 0;
	};

}

/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <boost/filesystem.hpp>

#include <Eve-Xin/Controllers/DataStore.h>

namespace Swift {
	class ApplicationPathProvider;
}

namespace EveXin {
	class FileDataStore : public DataStore {
		public:
			FileDataStore(boost::filesystem::path dataDir);
			virtual ~FileDataStore();

			virtual Swift::ByteArray getContent(const Swift::URL& url);
			virtual void setContent(const Swift::URL& url, const Swift::ByteArray& content);
		private:
			boost::filesystem::path urlToPath(const Swift::URL& url);
		private:
			boost::filesystem::path dataDir_;
	};

}

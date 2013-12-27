/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/FileDataStore.h>

#include <boost/filesystem/fstream.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Path.h>
#include <Swiften/StringCodecs/Base64.h>

#include <SwifTools/Application/ApplicationPathProvider.h>

namespace EveXin {

FileDataStore::FileDataStore(boost::filesystem::path dataDir) : dataDir_(dataDir) {

}

FileDataStore::~FileDataStore() {

}

Swift::ByteArray FileDataStore::getContent(const Swift::URL& url) {
	auto path = urlToPath(url);
	if (!boost::filesystem::exists(path)) {
		return Swift::createByteArray("");
	}
	Swift::ByteArray content;
	Swift::readByteArrayFromFile(content, path);
	return content;
}

void FileDataStore::setContent(const Swift::URL& url, const Swift::ByteArray& content) {
	boost::filesystem::path path = urlToPath(url);
	std::cerr << "Saving data to " << Swift::pathToString(path) << std::endl;
	try {
		boost::filesystem::ofstream file(path);
		file << Swift::byteArrayToString(content);
	}
	catch (const boost::filesystem::filesystem_error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
}

boost::filesystem::path FileDataStore::urlToPath(const Swift::URL& url) {
	std::string fileName = Swift::Base64::encode(Swift::createByteArray(url.toString())) + ".xml";
	return dataDir_ / fileName;
}

}

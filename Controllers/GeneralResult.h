/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Parser/Tree/ParserElement.h>

namespace Swift {
	class XMLParserFactory;
}

namespace EveXin {
	class GeneralResult {
		public:
			GeneralResult(const Swift::ByteArray& source, Swift::XMLParserFactory*);
			~GeneralResult();
			boost::posix_time::ptime getCachedUntil();
			boost::posix_time::ptime getDate();

			Swift::ParserElement::ref getResult();

			bool isValid();
			bool needsRefresh();

		private:
			boost::posix_time::ptime cachedUntil_;
			boost::posix_time::ptime date_;
			Swift::ParserElement::ref result_;
			bool valid_;
	};
}



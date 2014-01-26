/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace Swift {
	class NetworkFactories;
}

namespace EveXin {
	class DataController;

	class MainController {
		public:
			MainController(Swift::NetworkFactories* factories, const boost::filesystem::path& dataDir);
			~MainController();

			boost::shared_ptr<DataController> getDataController();

		private:
			Swift::NetworkFactories* factories_;
			boost::shared_ptr<DataController> dataController_;
	};

}

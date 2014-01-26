/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/program_options/options_description.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options/variables_map.hpp>

#include <QObject>

namespace Swift {
	class NetworkFactories;
}

namespace EveXin {
	class MainController;
	class QtMainWindow;

	class QtEveXin : public QObject {
		Q_OBJECT
		public:
			QtEveXin(Swift::NetworkFactories*, const boost::program_options::variables_map&);
			~QtEveXin();
		private:
			boost::shared_ptr<MainController> mainController_;
			QtMainWindow* mainWindow_;
	};
}

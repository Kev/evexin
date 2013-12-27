/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

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
			QtEveXin(Swift::NetworkFactories*);
			~QtEveXin();
		private:
			boost::shared_ptr<MainController> mainController_;
			QtMainWindow* mainWindow_;
	};
}

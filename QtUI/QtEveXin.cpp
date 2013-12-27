/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtEveXin.h>

#include <boost/make_shared.hpp>

#include <Eve-Xin/Controllers/MainController.h>

#include <Eve-Xin/QtUI/QtMainWindow.h>

namespace EveXin {

QtEveXin::QtEveXin(Swift::NetworkFactories* factories) {
	mainController_ = boost::make_shared<MainController>(factories);
	mainWindow_ = new QtMainWindow(mainController_->getDataController());
	mainWindow_->show();
}

QtEveXin::~QtEveXin() {
	delete mainWindow_;
}

}

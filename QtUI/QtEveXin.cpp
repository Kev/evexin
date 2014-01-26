/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/QtUI/QtEveXin.h>

#include <boost/make_shared.hpp>

#include <Swiften/Base/Path.h>

#include <SwifTools/Application/PlatformApplicationPathProvider.h>

#include <Eve-Xin/Controllers/MainController.h>

#include <Eve-Xin/QtUI/QtMainWindow.h>

namespace EveXin {

QtEveXin::QtEveXin(Swift::NetworkFactories* factories, const boost::program_options::variables_map& options) {
	Swift::PlatformApplicationPathProvider paths("Eve-Xin");
	boost::filesystem::path dataPath = paths.getDataDir();
	if (options.count("override-path") > 0) {
		dataPath = Swift::stringToPath(options["override-path"].as<std::string>());
	}
	mainController_ = boost::make_shared<MainController>(factories, dataPath);
	mainWindow_ = new QtMainWindow(mainController_->getDataController());
	mainWindow_->show();
}

QtEveXin::~QtEveXin() {
	delete mainWindow_;
}

}

/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/MainController.h>

#include <boost/make_shared.hpp>

#include <Eve-Xin/Controllers/DataController.h>

namespace EveXin {

MainController::MainController(Swift::NetworkFactories* factories) : factories_(factories) {
	dataController_ = boost::make_shared<DataController>(factories_);
}

MainController::~MainController() {
	
}

boost::shared_ptr<DataController> MainController::getDataController() {
	return dataController_;
}

}

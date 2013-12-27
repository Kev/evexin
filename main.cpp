/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QApplication>

#include <Swiften/EventLoop/Qt/QtEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>

#include <Eve-Xin/Controllers/MainController.h>

#include <Eve-Xin/QtUI/QtEveXin.h>

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	Swift::QtEventLoop eventLoop;
	Swift::BoostNetworkFactories factories(&eventLoop);
	EveXin::QtEveXin eveXin(&factories);
	return app.exec();
}

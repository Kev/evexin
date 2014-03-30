/*
 * Copyright (c) 2013-2014 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options.hpp>
#include <boost/version.hpp>

#include <QApplication>

#include <Swiften/EventLoop/Qt/QtEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>

#include <Eve-Xin/Controllers/MainController.h>

#include <Eve-Xin/QtUI/QtEveXin.h>

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	Swift::QtEventLoop eventLoop;
	Swift::BoostNetworkFactories factories(&eventLoop);

	boost::program_options::options_description desc("Options");
	desc.add_options()
		("help", "Show this help message")
		("debug", "Run in debug mode")
		("override-path", boost::program_options::value<std::string>(), "Override the default storage path with this one");
	boost::program_options::variables_map options;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), options);
	} catch (const boost::program_options::unknown_option& option) {
#if BOOST_VERSION >= 104200
		std::cout << "Ignoring unknown option " << option.get_option_name() << " but continuing." <<  std::endl;
#else
		std::cout << "Error: " << option.what() << " (continuing)" <<  std::endl;
#endif
	}
	boost::program_options::notify(options);

	if (options.count("help") > 0) {
		std::cout << "Eve-Xin is a skill planning utility for Eve Online." << std::endl;
		std::cout << std::endl;
		std::cout << "Usage: " << argv[0] << " [OPTIONS]..." << std::endl;
		std::cout << std::endl;
		std::cout << desc << std::endl;
		return 1;
	}

	EveXin::QtEveXin eveXin(&factories, options);
	return app.exec();
}

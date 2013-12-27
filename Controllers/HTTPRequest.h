/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/SafeByteArray.h>
#include <Swiften/Base/URL.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/Connection.h>

namespace Swift {
	class NetworkFactories;
	class TLSConnectionFactory;
}

namespace EveXin {
	class HTTPRequest : public boost::enable_shared_from_this<HTTPRequest> {
		public:
			typedef boost::shared_ptr<HTTPRequest> ref;
			HTTPRequest(const Swift::URL& url, Swift::NetworkFactories* factories);
			~HTTPRequest();
			void send();
		public:
			boost::signal<void()> onError;
			boost::signal<void(const Swift::ByteArray& data)> onComplete;
		private:
			void cancelConnector();
			void writeData();
			void handleConnectFinished(Swift::Connection::ref connection);
			void handleDataRead(boost::shared_ptr<Swift::SafeByteArray> data);
			void handleDisconnected(const boost::optional<Swift::Connection::Error>& error);
		private:
			Swift::URL url_;
			Swift::NetworkFactories* factories_;
			Swift::TLSConnectionFactory* tlsFactory_;
			Swift::SafeByteArray buffer_;
			Swift::Connection::ref connection_;
			Swift::Connector::ref connector_;
			bool receivedHeaders_;
	};

}

/*
 * Copyright (c) 2013 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Eve-Xin/Controllers/HTTPRequest.h>

#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Base/Concat.h>
#include <Swiften/Network/Connector.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Network/TLSConnectionFactory.h>

namespace EveXin {

HTTPRequest::HTTPRequest(const Swift::URL& url, Swift::NetworkFactories* factories) : url_(url), factories_(factories), receivedHeaders_(false) {
	std::string scheme = url.getScheme();
	boost::algorithm::to_lower(scheme);
	tlsFactory_ = (scheme != "https") ? NULL : new Swift::TLSConnectionFactory(factories->getTLSContextFactory(), factories->getConnectionFactory());
}

HTTPRequest::~HTTPRequest() {
	cancelConnector();
}

void HTTPRequest::send() {
	connector_ = Swift::Connector::create(url_.getHost(), Swift::URL::getPortOrDefaultPort(url_), false, factories_->getDomainNameResolver(), tlsFactory_ ? tlsFactory_ : factories_->getConnectionFactory(), factories_->getTimerFactory());
	connector_->onConnectFinished.connect(boost::bind(&HTTPRequest::handleConnectFinished, shared_from_this(), _1));
	connector_->start();
}

void HTTPRequest::handleConnectFinished(Swift::Connection::ref connection) {
	cancelConnector();
	connection_ = connection;
	if (connection) {
		connection_->onDataRead.connect(boost::bind(&HTTPRequest::handleDataRead, shared_from_this(), _1));
		connection_->onDisconnected.connect(boost::bind(&HTTPRequest::handleDisconnected, shared_from_this(), _1));
	}
	else {
		cancelConnector();
		onError();
		return;
	}
	writeData();
}

void HTTPRequest::cancelConnector() {
	if (connector_) {
		connector_->onConnectFinished.disconnect(boost::bind(&HTTPRequest::handleConnectFinished, shared_from_this(), _1));
		connector_->stop();
		connector_.reset();
	}
}

void HTTPRequest::writeData() {
//	size_t size = 0;
	std::stringstream header;
	header	<< "GET " << url_.getPath() << " HTTP/1.0\r\n"
			<< "Host: " << url_.getHost();
	if (url_.getPort()) {
			header << ":" << *(url_.getPort());
	}
	header	<< "\r\n"
		    << "User-Agent: " << "Eve-Xin(" << ")\r\n"
			// << "Accept-Encoding: deflate\r\n"
			// << "Content-Type: text/xml; charset=utf-8\r\n"
			// << "Content-Length: " << size << "\r\n"
			<<"\r\n";
	//std::cerr << ">>> " << header.str() << std::endl;
	connection_->write(Swift::createSafeByteArray(header.str()));
}

void HTTPRequest::handleDataRead(boost::shared_ptr<Swift::SafeByteArray> data) {
	//std::cerr << "<<< " << Swift::safeByteArrayToString(*data) << std::endl;
	buffer_ = Swift::concat(buffer_, *data);
	if (!receivedHeaders_) {
		std::string response = safeByteArrayToString(buffer_);
		if (response.find("\r\n\r\n") == std::string::npos) {
			return;
		}
		receivedHeaders_ = true;
		std::string httpCode = response.substr(response.find(" ") + 1, 3);
		if (httpCode != "200") {
			//onHTTPError(httpCode);
			onError();
			return;
		}
		std::string rest = Swift::safeByteArrayToString(buffer_);
		buffer_ = Swift::createSafeByteArray(response.substr(response.find("\r\n\r\n") + 4));
	}
	//Now see if we've had the headers (now, or previously)
	if (receivedHeaders_) {
	
	}
}

void HTTPRequest::handleDisconnected(const boost::optional<Swift::Connection::Error>& /*error*/) {
	//std::cerr << "HTTP Disconnected" << std::endl;
	cancelConnector();
	if (receivedHeaders_) {
		onComplete(Swift::createByteArray(safeByteArrayToString(buffer_))); // Yes, this is getting a little silly with the conversions.
	}
	else {
		onError();
	}
	connection_.reset(); // Delete the connection, the signals that keep the HTTPRequest alive get cleared, this gets cleaned up.
}

}



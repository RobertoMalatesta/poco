//
// SocketStream.cpp
//
// $Id: //poco/Main/Net/src/SocketStream.cpp#8 $
//
// Library: Net
// Package: Sockets
// Module:  SocketStream
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocketImpl.h"
#include "Poco/Exception.h"


using Poco::BufferedBidirectionalStreamBuf;
using Poco::InvalidArgumentException;


namespace Poco {
namespace Net {


//
// SocketStreamBuf
//


SocketStreamBuf::SocketStreamBuf(const Socket& socket): 
	BufferedBidirectionalStreamBuf(STREAM_BUFFER_SIZE, std::ios::in | std::ios::out),
	_pImpl(dynamic_cast<StreamSocketImpl*>(socket.impl()))
{
	if (_pImpl)
		_pImpl->duplicate(); 
	else
		throw InvalidArgumentException("Invalid or null SocketImpl passed to SocketStreamBuf");
}


SocketStreamBuf::~SocketStreamBuf()
{
	_pImpl->release();
}


int SocketStreamBuf::readFromDevice(char* buffer, std::streamsize length)
{
	return _pImpl->receiveBytes(buffer, (int) length);
}


int SocketStreamBuf::writeToDevice(const char* buffer, std::streamsize length)
{
	return _pImpl->sendBytes(buffer, (int) length);
}


//
// SocketIOS
//


SocketIOS::SocketIOS(const Socket& socket):
	_buf(socket)
{
	poco_ios_init(&_buf);
}


SocketIOS::~SocketIOS()
{
	try
	{
		_buf.sync();
	}
	catch (...)
	{
	}
}


SocketStreamBuf* SocketIOS::rdbuf()
{
	return &_buf;
}


void SocketIOS::close()
{
	_buf.sync();
	_buf.socketImpl()->close();
}


StreamSocket SocketIOS::socket() const
{
	return StreamSocket(_buf.socketImpl());
}


//
// SocketOutputStream
//


SocketOutputStream::SocketOutputStream(const Socket& socket):
	SocketIOS(socket),
	std::ostream(&_buf)
{
}


SocketOutputStream::~SocketOutputStream()
{
}


//
// SocketInputStream
//


SocketInputStream::SocketInputStream(const Socket& socket):
	SocketIOS(socket),
	std::istream(&_buf)
{
}


SocketInputStream::~SocketInputStream()
{
}


//
// SocketStream
//


SocketStream::SocketStream(const Socket& socket):
	SocketIOS(socket),
	std::iostream(&_buf)
{
}


SocketStream::~SocketStream()
{
}


} } // namespace Poco::Net

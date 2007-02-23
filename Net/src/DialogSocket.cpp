//
// DialogSocket.cpp
//
// $Id: //poco/Main/Net/src/DialogSocket.cpp#9 $
//
// Library: Net
// Package: Sockets
// Module:  DialogSocket
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


#include "Poco/Net/DialogSocket.h"
#include <string.h>
#include <ctype.h>


namespace Poco {
namespace Net {


DialogSocket::DialogSocket():
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


DialogSocket::DialogSocket(const SocketAddress& address): 
	StreamSocket(address),
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


DialogSocket::DialogSocket(const Socket& socket): 
	StreamSocket(socket),
	_pBuffer(0),
	_pNext(0),
	_pEnd(0)
{
	allocBuffer();
}


DialogSocket::~DialogSocket()
{
	delete [] _pBuffer;
}


DialogSocket& DialogSocket::operator = (const Socket& socket)
{
	StreamSocket::operator = (socket);
	return *this;
}


void DialogSocket::sendByte(unsigned char ch)
{
	sendBytes(&ch, 1);
}


void DialogSocket::sendString(const char* str)
{
	sendBytes(str, (int) strlen(str));
}


void DialogSocket::sendString(const std::string& str)
{
	sendBytes(str.data(), (int) str.length());
}


void DialogSocket::sendMessage(const std::string& message)
{
	std::string line;
	line.reserve(message.length() + 2);
	line.append(message);
	line.append("\r\n");
	sendString(line);
}


void DialogSocket::sendMessage(const std::string& message, const std::string& arg)
{
	std::string line;
	line.reserve(message.length() + arg.length() + 3);
	line.append(message);
	if (!arg.empty())
	{
		line.append(" ");
		line.append(arg);
	}
	line.append("\r\n");
	sendString(line);
}


void DialogSocket::sendMessage(const std::string& message, const std::string& arg1, const std::string& arg2)
{
	std::string line;
	line.reserve(message.length() + arg1.length() +arg2.length() + 4);
	line.append(message);
	line.append(" ");
	line.append(arg1);
	if (!arg2.empty())
	{
		line.append(" ");
		line.append(arg2);
	}
	line.append("\r\n");
	sendString(line);
}


bool DialogSocket::receiveMessage(std::string& message)
{
	message.clear();
	return receiveLine(message);
}


int DialogSocket::receiveStatusMessage(std::string& message)
{
	message.clear();
	int status = receiveStatusLine(message);
	if (status < 0)
	{
		while (status <= 0)
		{
			message += '\n';
			status = receiveStatusLine(message);
		}
	}
	return status;
}


int DialogSocket::get()
{
	refill();
	if (_pNext != _pEnd)
		return std::char_traits<char>::to_int_type(*_pNext++);
	else
		return EOF_CHAR;
}


int DialogSocket::peek()
{
	refill();
	if (_pNext != _pEnd)
		return std::char_traits<char>::to_int_type(*_pNext);
	else
		return EOF_CHAR;
}


void DialogSocket::synch()
{
	sendUrgent(TELNET_DM);
}


void DialogSocket::sendTelnetCommand(unsigned char command)
{
	unsigned char buffer[2];
	buffer[0] = TELNET_IAC;
	buffer[1] = command;
	sendBytes(buffer, 2);
}


void DialogSocket::sendTelnetCommand(unsigned char command, unsigned char arg)
{
	unsigned char buffer[3];
	buffer[0] = TELNET_IAC;
	buffer[1] = command;
	buffer[2] = arg;
	sendBytes(buffer, 3);
}


void DialogSocket::refill()
{
	if (_pNext == _pEnd)
	{
		int n = receiveBytes(_pBuffer, RECEIVE_BUFFER_SIZE);
		_pNext = _pBuffer;
		_pEnd  = _pBuffer + n;
	}
}


void DialogSocket::allocBuffer()
{
	_pBuffer = new char [RECEIVE_BUFFER_SIZE];
	_pNext   = _pBuffer;
	_pEnd    = _pBuffer;
}


bool DialogSocket::receiveLine(std::string& line)
{
	// An old wisdom goes: be strict in what you emit
	// and generous in what you accept.
	int ch = get();
	while (ch != EOF_CHAR && ch != '\r' && ch != '\n')
	{
		line += (char) ch;
		ch = get();
	}
	if (ch == '\r' && peek() == '\n')
		get();
	else if (ch == EOF_CHAR)
		return false;
	return true;
}


int DialogSocket::receiveStatusLine(std::string& line)
{
	int status = 0;
	int ch = get();
	if (ch != EOF_CHAR) line += (char) ch;
	int n = 0;
	while (isdigit(ch) && n < 3)
	{
		status *= 10;
		status += ch - '0';
		++n;
		ch = get();
		if (ch != EOF_CHAR) line += (char) ch;
	}
	if (n == 3)
	{
		if (ch == '-')
			status = -status;
	}
	else status = 0;
	if (ch != EOF_CHAR) receiveLine(line);
	return status;
}


} } // namespace Poco::Net

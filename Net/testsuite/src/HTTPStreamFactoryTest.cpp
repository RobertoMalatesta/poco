//
// HTTPStreamFactoryTest.cpp
//
// $Id: //poco/Main/Net/testsuite/src/HTTPStreamFactoryTest.cpp#8 $
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


#include "HTTPStreamFactoryTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/NetException.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "HTTPTestServer.h"
#include <sstream>
#include <memory>


using Poco::Net::HTTPStreamFactory;
using Poco::Net::NetException;
using Poco::Net::HTTPException;
using Poco::URI;
using Poco::StreamCopier;


HTTPStreamFactoryTest::HTTPStreamFactoryTest(const std::string& name): CppUnit::TestCase(name)
{
}


HTTPStreamFactoryTest::~HTTPStreamFactoryTest()
{
}


void HTTPStreamFactoryTest::testNoRedirect()
{
	HTTPTestServer server;
	HTTPStreamFactory factory;
	URI uri("http://localhost/large");
	uri.setPort(server.port());
	std::auto_ptr<std::istream> pStr(factory.open(uri));
	std::ostringstream ostr;
	StreamCopier::copyStream(*pStr.get(), ostr);
	assert (ostr.str() == HTTPTestServer::LARGE_BODY);
}


void HTTPStreamFactoryTest::testEmptyPath()
{
	HTTPTestServer server;
	HTTPStreamFactory factory;
	URI uri("http://localhost");
	uri.setPort(server.port());
	std::auto_ptr<std::istream> pStr(factory.open(uri));
	std::ostringstream ostr;
	StreamCopier::copyStream(*pStr.get(), ostr);
	assert (ostr.str() == HTTPTestServer::SMALL_BODY);
}


void HTTPStreamFactoryTest::testRedirect()
{
	HTTPTestServer server;
	HTTPStreamFactory factory;
	URI uri("http://localhost/redirect");
	uri.setPort(server.port());
	std::auto_ptr<std::istream> pStr(factory.open(uri));
	std::ostringstream ostr;
	StreamCopier::copyStream(*pStr.get(), ostr);
	assert (ostr.str() == HTTPTestServer::LARGE_BODY);
}


void HTTPStreamFactoryTest::testProxy()
{
	HTTPTestServer server;
	HTTPStreamFactory factory("localhost", server.port());
	URI uri("http://www.somehost.com/large");
	std::auto_ptr<std::istream> pStr(factory.open(uri));
	std::ostringstream ostr;
	StreamCopier::copyStream(*pStr.get(), ostr);
	assert (ostr.str() == HTTPTestServer::LARGE_BODY);
}


void HTTPStreamFactoryTest::testError()
{
	HTTPTestServer server;
	HTTPStreamFactory factory;
	URI uri("http://localhost/notfound");
	uri.setPort(server.port());
	try
	{
		std::istream* pStr = factory.open(uri);
		fail("not found - must throw");
	}
	catch (HTTPException& exc)
	{
		std::string m = exc.displayText();
	}
}


void HTTPStreamFactoryTest::setUp()
{
}


void HTTPStreamFactoryTest::tearDown()
{
}


CppUnit::Test* HTTPStreamFactoryTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("HTTPStreamFactoryTest");

	CppUnit_addTest(pSuite, HTTPStreamFactoryTest, testNoRedirect);
	CppUnit_addTest(pSuite, HTTPStreamFactoryTest, testEmptyPath);
	CppUnit_addTest(pSuite, HTTPStreamFactoryTest, testRedirect);
	CppUnit_addTest(pSuite, HTTPStreamFactoryTest, testProxy);
	CppUnit_addTest(pSuite, HTTPStreamFactoryTest, testError);

	return pSuite;
}

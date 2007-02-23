//
// MultipartWriterTest.cpp
//
// $Id: //poco/Main/Net/testsuite/src/MultipartWriterTest.cpp#6 $
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


#include "MultipartWriterTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/MultipartWriter.h"
#include "Poco/Net/MessageHeader.h"
#include <sstream>


using Poco::Net::MultipartWriter;
using Poco::Net::MessageHeader;


MultipartWriterTest::MultipartWriterTest(const std::string& name): CppUnit::TestCase(name)
{
}


MultipartWriterTest::~MultipartWriterTest()
{
}


void MultipartWriterTest::testWriteOnePart()
{
	std::ostringstream ostr;
	MultipartWriter w(ostr, "MIME_boundary_01234567");
	assert (w.boundary() == "MIME_boundary_01234567");
	MessageHeader h;
	h.set("name1", "value1");
	w.nextPart(h);
	ostr << "this is part 1";
	w.close();
	std::string s = ostr.str();	
	assert (s == "--MIME_boundary_01234567\r\nname1: value1\r\n\r\nthis is part 1\r\n--MIME_boundary_01234567--\r\n");
}


void MultipartWriterTest::testWriteTwoParts()
{
	std::ostringstream ostr;
	MultipartWriter w(ostr, "MIME_boundary_01234567");
	MessageHeader h;
	h.set("name1", "value1");
	w.nextPart(h);
	ostr << "this is part 1";
	h.clear();
	w.nextPart(h);
	ostr << "this is part 2";
	w.close();
	std::string s = ostr.str();	
	assert (s == "--MIME_boundary_01234567\r\nname1: value1\r\n\r\nthis is part 1\r\n--MIME_boundary_01234567\r\n\r\nthis is part 2\r\n--MIME_boundary_01234567--\r\n");
}


void MultipartWriterTest::testBoundary()
{
	std::ostringstream ostr;
	MultipartWriter w(ostr);
	std::string boundary = w.boundary();
	assert (boundary.substr(0, 14) == "MIME_boundary_");
	assert (boundary.length() == 14 + 16);
}


void MultipartWriterTest::setUp()
{
}


void MultipartWriterTest::tearDown()
{
}


CppUnit::Test* MultipartWriterTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("MultipartWriterTest");

	CppUnit_addTest(pSuite, MultipartWriterTest, testWriteOnePart);
	CppUnit_addTest(pSuite, MultipartWriterTest, testWriteTwoParts);
	CppUnit_addTest(pSuite, MultipartWriterTest, testBoundary);

	return pSuite;
}

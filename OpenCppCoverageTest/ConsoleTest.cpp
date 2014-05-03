#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include "TestCoverageConsole/TestCoverageConsole.hpp"

#include "tools/ScopedAction.hpp"

namespace fs = boost::filesystem;

namespace OpenCppCoverageTest
{	
	//-------------------------------------------------------------------------
	TEST(ConsoleTest, HtmlExport)
	{
		fs::path console = fs::canonical("../Debug/OpenCppCoverageTest.exe"); // $$ clean this code
		fs::path testExe = TestCoverageConsole::GetOutputBinaryPath();

		std::vector<std::string> arguments{ testExe.string() };
		Poco::Pipe output;
		auto handle = Poco::Process::launch(console.string(), arguments, "../Debug", nullptr, &output, &output);
		handle.wait(); // $$ check return value

		Poco::PipeInputStream inputStream(output);
		Poco::StreamCopier::copyStream(inputStream, std::cout);
	}
}
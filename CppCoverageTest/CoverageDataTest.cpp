// OpenCppCoverage is an open source code coverage for C++.
// Copyright (C) 2014 OpenCppCoverage
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"

#include "CppCoverage/CoverageData.hpp"
#include "CppCoverage/ModuleCoverage.hpp"
#include "CppCoverage/FileCoverage.hpp"
#include "CppCoverage/LineCoverage.hpp"

namespace cov = CppCoverage;

namespace CppCoverageTest
{
	namespace
	{
		const std::wstring moduleName = L"moduleName";
		const std::wstring filename = L"filename";

		//---------------------------------------------------------------------
		void FillCoverageData(cov::CoverageData& data)
		{
			auto& createdModule = data.AddModule(moduleName);
			auto& createdFile = createdModule.AddFile(filename);
			createdFile.AddLine(1, true);
			createdFile.AddLine(2, false);
		}

		//---------------------------------------------------------------------
		void CheckCoverageData(const cov::CoverageData& data)
		{
			const auto& modules = data.GetModules();
			ASSERT_EQ(1, modules.size());

			const auto& module = *modules.front();
			ASSERT_EQ(moduleName, module.GetPath());

			const auto& files = module.GetFiles();
			ASSERT_EQ(1, files.size());

			const auto& file = *files.front();
			ASSERT_EQ(filename, file.GetPath());
			ASSERT_EQ(nullptr, file[0]);

			const auto* line1 = file[1];
			const auto* line2 = file[2];

			ASSERT_NE(nullptr, line1);
			ASSERT_TRUE(line1->HasBeenExecuted());
			ASSERT_NE(nullptr, line2);
			ASSERT_FALSE(line2->HasBeenExecuted());
		}

		//---------------------------------------------------------------------
		void CreateNewFileCoverage(
			cov::ModuleCoverage& moduleCoverage,
			const std::wstring& fileName,
			int executedLine,
			int nonExecutedLine)
		{
			auto& file = moduleCoverage.AddFile(fileName);
			int line = 0;

			for (int i = 0; i < executedLine; ++i, ++line)
				file.AddLine(line, true);

			for (int i = 0; i < nonExecutedLine; ++i, ++line)
				file.AddLine(line, false);
			file.ComputeCoverageRate();
		}

		//---------------------------------------------------------------------
		template<typename T>
		void CheckCoverageRate(
			const T& value,
			int executedExpectedCount,
			int totalLineCount)
		{
			const auto& coverageRate = value.GetCoverageRate();

			ASSERT_EQ(executedExpectedCount, coverageRate.GetExecutedLinesCount());
			ASSERT_EQ(totalLineCount, coverageRate.GetTotalLinesCount());
		}
	}

	//---------------------------------------------------------------------
	TEST(CoverageDataTest, CoverageData)
	{
		cov::CoverageData data{L"", 0};
		
		FillCoverageData(data);
		CheckCoverageData(data);
	}

	//---------------------------------------------------------------------
	TEST(CoverageDataTest, MoveConstructor)
	{
		cov::CoverageData data{L"", 0};

		FillCoverageData(data);

		cov::CoverageData movedCoverageData = { std::move(data) };
		CheckCoverageData(movedCoverageData);
	}
		
	//---------------------------------------------------------------------
	TEST(CoverageDataTest, CoverageRate)
	{
		cov::CoverageData data{ L"", 0};

		auto& module1 = data.AddModule(L"module1");
		CreateNewFileCoverage(module1, L"filename1", 1, 2);			
				
		auto& module2 = data.AddModule(L"module1");
		CreateNewFileCoverage(module2, L"filename2", 3, 0);
		CreateNewFileCoverage(module2, L"filename3", 1, 1);		
		
		data.ComputeCoverageRate();

		CheckCoverageRate(module1, 1, 3); 
		CheckCoverageRate(module2, 4, 5);		
		CheckCoverageRate(data, 5, 8);
	}
}
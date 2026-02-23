// FPGA Placement.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <vector>
#include <chrono>
#include "FilleParser.h"
#include "Resource.h"
#include "Instance.h"
#include "PlacementAlgorithm.h"


int main(int argc, char* argv[]) {

	std::vector<std::string> allArgs(argv, argv + argc);
	if (allArgs.size() < 5) {
		throw std::runtime_error("Error: Please input arguments.");
	}

	std::string inputArchitectureFilePath = allArgs.at(1);
	std::cout << "input file architecture path = " << inputArchitectureFilePath << std::endl;

	std::string inputInstanceFilePath = allArgs.at(2);
	std::cout << "input file instance path = " << inputInstanceFilePath << std::endl;

	std::string inputNetFilePath = allArgs.at(3);
	std::cout << "input file net path = " << inputNetFilePath << std::endl;

	std::string outputFilePath = allArgs.at(4);
	std::cout << "outputFilePath file path = " << outputFilePath << std::endl;

	std::shared_ptr <Placement> placement = std::make_shared<Placement>();
	std::chrono::high_resolution_clock::time_point startPoint = std::chrono::high_resolution_clock::now();

    FileParser::initializeResoures(placement, inputArchitectureFilePath);
	FileParser::initializeInstances(placement, inputInstanceFilePath);
	FileParser::initializeNets(placement, inputNetFilePath);

	PlacementAlgorithm::legalizeByMinimunDisplacement(placement);
	PlacementAlgorithm::computeWirelength(placement);

	PlacementAlgorithm::detailedPlacement(placement, startPoint);
	PlacementAlgorithm::computeWirelength(placement);
	FileParser::writeOutputFile(placement, outputFilePath);
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示:
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案

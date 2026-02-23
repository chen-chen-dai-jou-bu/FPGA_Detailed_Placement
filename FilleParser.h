#pragma once
#ifndef FILE_PARSER
#define FILE_PARSER

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cfloat>
#include <cmath>
#include "Placement.h"
#include "Resource.h"
#include "Instance.h"
#include "Net.h"

class FileParser {

	public:
		/*
		 *  Define : Break a string into a vector by delimiter
		 */
		static std::vector<std::string> splitString(std::string line) {

			std::vector<std::string> words = {};

			std::stringstream linestream(line);
			std::string word;
			while (linestream >> word) {
				words.push_back(word);
			}

			return words;
		}

        static void initializeResoures(std::shared_ptr<Placement> placement, std::string filePath) {

            std::fstream file;
            file.open(filePath, std::ios::in);
            if (!file) {
                throw std::runtime_error("Error: File is not exist.");
            }

            std::vector<std::shared_ptr<Resource>> resources = {};
			std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Resource>>> resourceTable = {};

            // Line Example : ¡uRESOURCE1 CLB 1.5 0.5¡v
			const int INDEX_NAME = 0;
            const int INDEX_TYPE = 1;
            const int INDEX_COORDINATE_X = 2;
            const int INDEX_COORDINATE_Y = 3;

			double maximunX = DBL_MIN;
			double maximunY = DBL_MIN;

			// Checking whether the file is open.
			if (file.is_open()) {

				std::string line;
				// Read data from the file object and put it into a string.
				while (getline(file, line)) {
					std::vector<std::string> vector = FileParser::splitString(line);

					std::string name = std::string(vector.at(INDEX_NAME));
					std::string type = std::string(vector.at(INDEX_TYPE));
					double coordinateX = std::stod(vector.at(INDEX_COORDINATE_X));
					double coordinateY = std::stod(vector.at(INDEX_COORDINATE_Y));

					std::shared_ptr<Resource> resoucre = std::make_shared<Resource>(name, type, coordinateX, coordinateY);
					resources.push_back(resoucre);
					resourceTable[type][name] = resoucre;

					if (coordinateX > maximunX) {
						maximunX = coordinateX;
					}
					if (coordinateY > maximunY) {
						maximunY = coordinateY;
					}
				}

				// Close the file object.
				file.close();
			}

			placement->setResources(resources);
			placement->setResourceTable(resourceTable);

			// 2-D Map about resources
			std::vector<std::vector<std::shared_ptr<Resource>>> resourceMap(std::ceil(maximunY), std::vector<std::shared_ptr<Resource>>(std::ceil(maximunX)));
			for (const auto& i : resources) {
				resourceMap[(int)i->getCoordinateY()][(int)i->getCoordinateX()] = i;
			}
			placement->setResourceMap(resourceMap, std::ceil(maximunX), std::ceil(maximunY));
        }

		static void initializeInstances(std::shared_ptr<Placement> placement, std::string filePath) {

			std::fstream file;
			file.open(filePath, std::ios::in);
			if (!file) {
				throw std::runtime_error("Error: File is not exist.");
			}

			std::vector<std::shared_ptr<Instance>> instnaces = {};
			std::unordered_map<std::string, std::shared_ptr<Instance>> instnaceTable = {};

			// Line Example : ¡uCLB106 CLB 106.33 57.28¡v
			const int INDEX_NAME = 0;
			const int INDEX_TYPE = 1;
			const int INDEX_COORDINATE_X = 2;
			const int INDEX_COORDINATE_Y = 3;

			// Checking whether the file is open.
			if (file.is_open()) {

				std::string line;
				// Read data from the file object and put it into a string.
				while (getline(file, line)) {
					std::vector<std::string> vector = FileParser::splitString(line);

					std::string name = std::string(vector.at(INDEX_NAME));
					std::string type = std::string(vector.at(INDEX_TYPE));
					double coordinateX = std::stod(vector.at(INDEX_COORDINATE_X));
					double coordinateY = std::stod(vector.at(INDEX_COORDINATE_Y));

					std::shared_ptr<Instance> instnace = std::make_shared<Instance>(name, type, coordinateX, coordinateY);
					instnaces.push_back(instnace);
					instnaceTable[name] = instnace;
				}

				// Close the file object.
				file.close();
			}

			placement->setInstances(instnaces);
			placement->setInstnaceTable(instnaceTable);
		}

		static void initializeNets(std::shared_ptr<Placement> placement, std::string filePath) {

			std::fstream file;
			file.open(filePath, std::ios::in);
			if (!file) {
				throw std::runtime_error("Error: File is not exist.");
			}

			// Line Example : ¡unet_3333 CLB107 CLB4 CLB57 CLB59 CLB8¡v
			const int INDEX_NAME = 0;
			std::vector<std::shared_ptr<Net>> nets = {};

			// Checking whether the file is open.
			if (file.is_open()) {

				std::string line;

				// Read data from the file object and put it into a string.
				while (getline(file, line)) {
					std::vector<std::string> vector = FileParser::splitString(line);

					// Line Example : ¡unet_3240 CLB41¡v
					if (vector.size() < 3) {
						continue;
					}

					std::string netName = std::string(vector.at(INDEX_NAME));
					std::shared_ptr<Net> net = std::make_shared<Net>(netName);

					std::vector<std::string> instances = {};
					for (int i = 1; i < vector.size(); i++) {
						std::string instanceName = std::string(vector.at(i));
						instances.push_back(instanceName);

						std::shared_ptr<Instance> inst = placement->getInstanceByName(instanceName);
						inst->addNet(net);
					}

					net->setInstances(instances);
					nets.push_back(net);
				}

				// Close the file object.
				file.close();
			}

			placement->setNets(nets);
		}

		// Check file if exist. if it exists, giving another name
		static std::string getOutputFileName(std::string outputFilePath) {

			std::filesystem::path relativePath(outputFilePath);
			std::string fileName = relativePath.stem().string();
			std::string extensionName = relativePath.extension().string();
			std::string newFileName = fileName;
			int index = 0;
			while (std::ifstream(outputFilePath)) {
				index++;
				newFileName = fileName;
				newFileName += "-";
				newFileName += std::to_string(index);
				newFileName += extensionName;
				outputFilePath = relativePath.remove_filename().string() + newFileName;
			}
			return outputFilePath;

		}

		static void writeOutputFile(std::shared_ptr<Placement> placement, std::string filePath) {

			std::string newOutputPath = getOutputFileName(filePath);
			std::ofstream outfile(filePath);

			if (!outfile.is_open()) {
				throw std::runtime_error("Error: Couldn't open output file");
			}

			// write all instance mapping
			for (auto& i : placement->getInstances()) {

				if (i->getType() == "IO") {
					continue;
				}
				outfile << i->getName() << " " << i->getMappingResourceName() << std::endl;
			}

			outfile.close();
		}
};

#endif /* FILE_PARSER */
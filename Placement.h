#pragma once
#ifndef PLACEMENT_H
#define PLACEMENT_H

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include "Resource.h"
#include "Instance.h"
#include "Net.h"

class Placement {

	private:
		std::vector<std::shared_ptr<Instance>> instnaces = {};
		std::unordered_map<std::string, std::shared_ptr<Instance>> instnaceTable = {};

		std::vector<std::shared_ptr<Resource>> resources = {};
		std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Resource>>> resourceTable = {};

		std::vector<std::shared_ptr<Net>> nets = {};

		std::vector<std::vector<std::shared_ptr<Resource>>> resourceMap = {};
		int maximunX = 0;
		int maximunY = 0;
	public:
		void setInstances(const std::vector<std::shared_ptr<Instance>>& _instances) {
			this->instnaces = _instances;
		}

		std::vector<std::shared_ptr<Instance>> getInstances() const {
			return this->instnaces;
		}

		void setInstnaceTable(const std::unordered_map<std::string, std::shared_ptr<Instance>>& _instanceTable) {
			this->instnaceTable = _instanceTable;
		}

		std::unordered_map<std::string, std::shared_ptr<Instance>> getInstanceTable() const {
			return this->instnaceTable;
		}

		std::shared_ptr<Instance> getInstanceByName(const std::string& _name) {
			return this->instnaceTable[_name];
		}

		void setResources(const std::vector<std::shared_ptr<Resource>>& _resources) {
			this->resources = _resources;
		}

		std::vector<std::shared_ptr<Resource>> getResources() const {
			return this->resources;
		}

		void setResourceTable(const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Resource>>>& _resourcesTable) {
			this->resourceTable = _resourcesTable;
		}

		void setResourceMap(const std::vector<std::vector<std::shared_ptr<Resource>>>& _resourceMap, int _maximunX, int _maximunY) {
			this->resourceMap = _resourceMap;
			this->maximunX = _maximunX;
			this->maximunY = _maximunY;
		}

		std::shared_ptr<Resource> getResourceByCoordinate(int x, int y) {
			if (x < 0 || y < 0) {
				return nullptr;
			}
			if (x > maximunX - 1 || y > maximunY - 1) {
				return nullptr;
			}

			return resourceMap[y][x];
		}

		std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Resource>>> getResourceTable() const {
			return this->resourceTable;
		}

		std::unordered_map<std::string, std::shared_ptr<Resource>> getResourcesByType(const std::string& type) {
			return this->resourceTable[type];

		}

		void removeResourceFromTable(const std::string& type, const std::string& resourceName) {
			this->resourceTable[type].erase(resourceName);
		}

		void setNets(const std::vector<std::shared_ptr<Net>>& _nets) {
			this->nets = _nets;
		}

		std::vector<std::shared_ptr<Net>> getNets() const {
			return this->nets;
		}
};

#endif /* PLACEMENT_H */
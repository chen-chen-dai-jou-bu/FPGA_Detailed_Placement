#pragma once
#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include "Net.h"

class Instance {

	private:
		std::string name;
		std::string type;
		double coordinateX;
		double coordinateY;

		std::string mappingResourceName = "";
		std::vector<std::shared_ptr<Net>> nets = {};

	public:
		Instance(const std::string& _name, const std::string& _type, double _coordinateX, double _coordinateY) {
			this->name = _name;
			this->type = _type;
			this->coordinateX = _coordinateX;
			this->coordinateY = _coordinateY;
		}

		void setName(const std::string& _name) {
			this->name = _name;
		}

		std::string getName() const {
			return this->name;
		}

		void setType(std::string _type) {
			this->type = _type;
		}

		std::string getType() {
			return this->type;
		}

		void setCoordinateX(double _coordinateX) {
			this->coordinateX = _coordinateX;
		}

		double getCoordinateX() {
			return this->coordinateX;
		}

		void setCoordinateY(double _coordinateY) {
			this->coordinateY = _coordinateY;
		}

		double getCoordinateY() {
			return this->coordinateY;
		}

		void setMappingResourceName(const std::string& _resourceName) {
			this->mappingResourceName = _resourceName;
		}

		std::string getMappingResourceName() {
			return this->mappingResourceName;
		}

		void addNet(std::shared_ptr<Net> _net) {
			this->nets.push_back(_net);
		}

		std::vector<std::shared_ptr<Net>> getNets() {
			return this->nets;
		}
};

#endif /* INSTANCE_H */
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include "Instance.h"

class Resource {

	private:
		std::string name;
		std::string type;
		double coordinateX;
		double coordinateY;
		bool isMapped = false;
		std::shared_ptr<Instance> instance;

	public:
		Resource(const std::string& _name, const std::string& _type, double _coordinateX, double _coordinateY) {
			this->name = _name;
			this->type = _type;
			this->coordinateX = _coordinateX;
			this->coordinateY = _coordinateY;
		}

		std::string getName() const {
			return name;
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

		bool getIsMapped() {
			return this->isMapped;
		}

		void setIsMapped(bool _isMapped) {
			this->isMapped = _isMapped;
		}

		void setInstance(const std::shared_ptr<Instance>& _instance) {
			this->instance = _instance;
		}

		std::shared_ptr<Instance> getInstance() const {
			return this->instance;
		}
};

#endif /* RESOURCE_H */
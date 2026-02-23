#pragma once
#ifndef NET_H
#define NET_H

#include <vector>
#include <string>

class Net {

	private:
		std::string name;
		std::vector<std::string> instances = {};

	public:
		Net(std::string _name) {
			this->name = _name;
		}

		Net(std::string _name, std::vector<std::string>& _instances) {
			this->name = _name;
			this->instances = _instances;
		}

		void setInstances(std::vector<std::string>& _instances) {
			this->instances = _instances;
		}

		std::vector<std::string> getInstances() const {
			return instances;
		}
};

#endif /* NET_H */
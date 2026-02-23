#pragma once
#ifndef PLACEMENT_ALGORITHM_H
#define PLACEMENT_ALGORITHM_H

#include <cfloat>
#include <cmath>
#include <chrono>
#include "Placement.h"

class PlacementAlgorithm {

	public:
		// Mapping positions from resources one by one within minimum distance
		static void legalizeByMinimunDisplacement(std::shared_ptr<Placement> placement) {

			for (auto& instance : placement->getInstances()) {

				std::shared_ptr<Resource> resource = nullptr;
				double minimunDistance = DBL_MAX;

				std::string type = instance->getType();
				if (type == "IO") {
					continue;
				}

				// Find the minimun distance from available resources
				for (const auto& i : placement->getResourcesByType(type)) {
					std::shared_ptr<Resource> candidate = i.second;

					double distance = abs(candidate->getCoordinateX() - instance->getCoordinateX()) + abs(candidate->getCoordinateY() - instance->getCoordinateY());
					if (distance < 1.0) {
						resource = candidate;
						break;
					}
					if (distance < minimunDistance) {
						resource = candidate;
						minimunDistance = distance;
					}
				}

				// mapping instance to the resource
				instance->setMappingResourceName(resource->getName());
				instance->setCoordinateX(resource->getCoordinateX());
				instance->setCoordinateY(resource->getCoordinateY());
				resource->setIsMapped(true);
				resource->setInstance(instance);
				placement->removeResourceFromTable(type, resource->getName());
			}
		}

		// We hypothetically calculate the HPWL (Half-Perimeter Wirelength) when an instance moves to a new position.
		static double computeHPWL(std::shared_ptr<Placement> placement, std::shared_ptr<Instance> inst, double x, double y) {

			double wirelength = 0.0;
			for (const auto& net : inst->getNets()) {

				double maximunX = DBL_MIN;
				double maximunY = DBL_MIN;
				double minimunX = DBL_MAX;
				double minimunY = DBL_MAX;

				for (const auto& i : net->getInstances()) {
					if (i == inst->getName()) {
						continue;
					}
					std::shared_ptr<Instance> otherInst = placement->getInstanceByName(i);
					maximunX = std::max(maximunX, otherInst->getCoordinateX());
					maximunY = std::max(maximunY, otherInst->getCoordinateY());
					minimunX = std::min(minimunX, otherInst->getCoordinateX());
					minimunY = std::min(minimunY, otherInst->getCoordinateY());
				}

				maximunX = std::max(maximunX, x);
				maximunY = std::max(maximunY, y);
				minimunX = std::min(minimunX, x);
				minimunY = std::min(minimunY, y);

				wirelength += (maximunY - minimunY) + (maximunX - minimunX);
			}

			return wirelength;
		}

		static double computeHPWL(std::shared_ptr<Placement> placement, std::shared_ptr<Instance> inst) {

			double wirelength = 0.0;
			for (auto& net : inst->getNets()) {

				double maximunX = DBL_MIN;
				double maximunY = DBL_MIN;
				double minimunX = DBL_MAX;
				double minimunY = DBL_MAX;

				for (auto& i : net->getInstances()) {

					std::shared_ptr<Instance> otherInst = placement->getInstanceByName(i);
					maximunX = std::max(maximunX, otherInst->getCoordinateX());
					maximunY = std::max(maximunY, otherInst->getCoordinateY());
					minimunX = std::min(minimunX, otherInst->getCoordinateX());
					minimunY = std::min(minimunY, otherInst->getCoordinateY());
				}

				wirelength += (maximunY - minimunY) + (maximunX - minimunX);
			}

			return wirelength;
		}

		static std::shared_ptr<Resource> findCandidate(std::shared_ptr<Placement> placement, std::shared_ptr<Instance> instance) {

			double currentHPWL = computeHPWL(placement, instance);

			std::shared_ptr<Resource> candicate = nullptr;
			double maximumGain = 0.0;

			const int expandableRange = 2;
			int maximunX = (int) instance->getCoordinateX() + expandableRange;
			int maximunY = (int) instance->getCoordinateY() + expandableRange;
			int minimunX = (int) instance->getCoordinateX() - expandableRange;
			int minimunY = (int) instance->getCoordinateY() - expandableRange;

			for (int x = minimunX; x < maximunX; x++) {
				for (int y = minimunY; y < maximunY; y++) {

					if (x == (int) instance->getCoordinateX() && y == (int) instance->getCoordinateY()) {
						continue;
					}

					std::shared_ptr<Resource> resource = placement->getResourceByCoordinate(x, y);
					if (resource != nullptr) {
						if (resource->getType() == instance->getType()) {

							// Resource is not occupied by others
							if (!resource->getIsMapped()) {
								double moveHPWL = computeHPWL(placement, instance, resource->getCoordinateX(), resource->getCoordinateY());
								double gain = currentHPWL - moveHPWL;
								if (gain > maximumGain) {
									candicate = resource;
									maximumGain = gain;
								}
							}
							else {
								// std::cout << "Resource is occupied by others" << std::endl;
								std::shared_ptr<Instance> otherInstance = resource->getInstance();
								double otherHPWL = computeHPWL(placement, otherInstance);
								double otherMoveHPWL = computeHPWL(placement, otherInstance, instance->getCoordinateX(), instance->getCoordinateY());

								double moveHPWL = computeHPWL(placement, instance, resource->getCoordinateX(), resource->getCoordinateY());
								double gain = (otherHPWL - otherMoveHPWL) + (currentHPWL - moveHPWL);
								if (gain > maximumGain) {
									candicate = resource;
									maximumGain = gain;
								}
							} // end if-else
						}
					} // end if-else
				}
			} // end for-loop

			return candicate;
		}

		static void detailedPlacement(std::shared_ptr<Placement> placement, std::chrono::high_resolution_clock::time_point runningStart) {

			std::chrono::milliseconds timeLimit(580000); // 9 minutes 40 seconds
			for (const auto&instance : placement->getInstances()) {

				if (instance->getType() == "IO") {
					continue;
				}

				// This candidate will reduce original HPWL
				std::shared_ptr<Resource> candicate = findCandidate(placement, instance);
				if (candicate != nullptr) {

					if (candicate->getIsMapped()) {
						// SWAP to a non-empty position
						std::shared_ptr<Resource> originalResource = placement->getResourceByCoordinate((int)instance->getCoordinateX(), (int)instance->getCoordinateY());
						std::shared_ptr<Instance> otherInstance = candicate->getInstance();
						originalResource->setInstance(candicate->getInstance());
						otherInstance->setMappingResourceName(originalResource->getName());
						otherInstance->setCoordinateX(originalResource->getCoordinateX());
						otherInstance->setCoordinateY(originalResource->getCoordinateY());

						instance->setMappingResourceName(candicate->getName());
						instance->setCoordinateX(candicate->getCoordinateX());
						instance->setCoordinateY(candicate->getCoordinateY());
						candicate->setInstance(instance);
					}
					else {
						// SWAP to a empty position
						std::shared_ptr<Resource> originalResource = placement->getResourceByCoordinate((int)instance->getCoordinateX(), (int)instance->getCoordinateY());
						originalResource->setIsMapped(false);
						originalResource->setInstance(nullptr);

						instance->setMappingResourceName(candicate->getName());
						instance->setCoordinateX(candicate->getCoordinateX());
						instance->setCoordinateY(candicate->getCoordinateY());
						candicate->setIsMapped(true);
						candicate->setInstance(instance);
					}
				}

				std::chrono::duration<double, std::milli> runningTime = std::chrono::high_resolution_clock::now() - runningStart;
				if (runningTime > timeLimit) {
					// cout << "Execution timeout :" << runningTime.count() << "ms" << std::endl;
					break;
				}
			} // end for-loop
		}

		static double computeWirelength(std::shared_ptr<Placement> placement) {

			double wirelength = 0.0;
			for (auto& net : placement->getNets()) {

				double maximunX = DBL_MIN;
				double maximunY = DBL_MIN;
				double minimunX = DBL_MAX;
				double minimunY = DBL_MAX;

				for (auto& i : net->getInstances()) {
					std::shared_ptr<Instance> inst = placement->getInstanceByName(i);
					maximunX = std::max(maximunX, inst->getCoordinateX());
					maximunY = std::max(maximunY, inst->getCoordinateY());
					minimunX = std::min(minimunX, inst->getCoordinateX());
					minimunY = std::min(minimunY, inst->getCoordinateY());
				}

				wirelength += (maximunY - minimunY) + (maximunX - minimunX);
			}
			std::cout << wirelength << std::endl;
			return wirelength;
		}

};

#endif /* PLACEMENT_ALGORITHM_H */
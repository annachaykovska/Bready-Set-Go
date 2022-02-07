#include "PathFinder.h"

namespace
{
	double PI = 3.141592653589;
	double EPSILON = 0.00001;
}

PathFinder::PathFinder(NavMesh& navMesh) : navMesh_(navMesh)
{
	updateTraversablePathSteps();
}

void PathFinder::updateTraversablePathSteps()
{
	traversablePathSteps_.clear();

	for (int i = 0; i < navMesh_.getSegments().size(); i++)
	{
		NavMesh::MeshSegment* segment = navMesh_.getSegments().at(i);
		if (segment->traversable_)
		{
			PathStep* step1 = new PathStep();
			PathStep* step2 = new PathStep();
			PathStep* step3 = new PathStep();

			step1->segment.second = segment;
			step2->segment.second = segment;
			step3->segment.second = segment;

			step1->segment.first = segment->v0_;
			step2->segment.first = segment->v1_;
			step3->segment.first = segment->v2_;

			traversablePathSteps_.push_back(step1);
			traversablePathSteps_.push_back(step2);
			traversablePathSteps_.push_back(step3);
		}
	}
}

NavMesh::MeshSegment* PathFinder::findStartingSegment(position start)
{
	for (int i = 0; i < navMesh_.getSegments().size(); i++)
	{
		float summedAngle = 0;

		glm::vec3 vectorA = navMesh_.getSegments().at(i)->v0_->position_ - start;
		glm::vec3 vectorB = navMesh_.getSegments().at(i)->v1_->position_ - start;
		glm::vec3 vectorC = navMesh_.getSegments().at(i)->v2_->position_ - start;

		summedAngle += acos(dot(vectorA, vectorB) / (length(vectorA) * length(vectorB)));
		summedAngle += acos(dot(vectorA, vectorC) / (length(vectorA) * length(vectorC)));
		summedAngle += acos(dot(vectorB, vectorC) / (length(vectorB) * length(vectorC)));

		if (abs(summedAngle - 2 * PI) < EPSILON)
		{
			return navMesh_.getSegments().at(i);
		}
	}
	return nullptr;
}

PathStep* PathFinder::lowestFCost()
{
	double lowCost = std::numeric_limits<double>::infinity();
	PathStep* lowPathStep = nullptr;

	for (int i = 0; i < open_.size(); i++)
	{
		if (open_.at(i)->fCost < lowCost)
		{
			lowCost = open_.at(i)->fCost;
			lowPathStep = open_.at(i);
		}
	}
	return lowPathStep;
}

float PathFinder::calculateAndUpdateFCost(position end, PathStep* current, PathStep* neighbor)
{
	float hCost = length(end - neighbor->segment.first->position_);
	float gCost = length(neighbor->segment.first->position_ - current->segment.first->position_) + current->gCost;
	neighbor->gCost = gCost;
	return hCost + gCost;
}

int PathFinder::indexOfPathStep(PathStep* step)
{
	for (int i = 0; i < open_.size(); i++)
	{
		if (open_.at(i) == step)
		{
			return i;
		}
	}
	return -1;
}

bool PathFinder::stepContainsTarget(position target, PathStep* step)
{
	float summedAngle = 0;
	glm::vec3 vectorA = step->segment.second->v0_->position_ - target;
	glm::vec3 vectorB = step->segment.second->v1_->position_ - target;
	glm::vec3 vectorC = step->segment.second->v2_->position_ - target;

	summedAngle += acos(dot(vectorA, vectorB) / (length(vectorA) * length(vectorB)));
	summedAngle += acos(dot(vectorA, vectorC) / (length(vectorA) * length(vectorC)));
	summedAngle += acos(dot(vectorB, vectorC) / (length(vectorB) * length(vectorC)));

	if (abs(summedAngle - 2 * PI) < EPSILON)
	{
		return true;
	}
	return false;
}

std::vector<position> PathFinder::findPath(position start, position end)
{
	open_.clear();
	closed_.clear();

	NavMesh::MeshSegment* startingSegment = findStartingSegment(start);

	// Add the start node to OPEN
	for (int i = 0; i < traversablePathSteps_.size(); i++)
	{
		if (traversablePathSteps_.at(i)->segment.second == startingSegment)
		{
			traversablePathSteps_.at(i)->fCost = length(traversablePathSteps_.at(i)->segment.first->position_ - start);
			open_.push_back(traversablePathSteps_.at(i));
		}
	}

	PathStep* current = nullptr;
	PathStep* previous;

	while (1)
	{
		// current = node in OPEN with the lowest f_cost
		previous = current;
		current = lowestFCost();
		if (open_.size() != 0)
		{
			// remove current from OPEN
			open_.erase(open_.begin() + indexOfPathStep(current));

			// add current to CLOSED
			closed_.push_back(current);
		}

		if (current == nullptr)
		{
			printPath(previous);
			std::vector<position> empty;
			return empty;
		}

		// if current is the target node // path has been found
		if (stepContainsTarget(end, current))
		{
			std::cout << "Done" << std::endl;
			printPath(current);

			std::vector<position> finalPath;

			while (current->parent != nullptr)
			{
				finalPath.push_back(current->segment.first->position_);
				current = current->parent;
			}
			finalPath.push_back(current->segment.first->position_);
			
			return finalPath;
		}

		// for each neighbour of the current node
		for (pathSegment neighbor : current->segment.second->neighbors)
		{
			bool skip = false;
			PathStep* neighborStep = nullptr;

			for (PathStep* step : closed_)
			{
				if (step->segment == neighbor)
				{
					skip = true;
				}
			}

			if (!skip)
			{
				bool isOpen = false;
				for (PathStep* step : open_)
				{
					if (step->segment == neighbor)
					{
						isOpen = true;
					}
				}

				for (PathStep* step : traversablePathSteps_)
				{
					if (step->segment == neighbor)
					{
						neighborStep = step;
						break;
					}
				}

				float neighborFCost = calculateAndUpdateFCost(end, current, neighborStep);
				if (neighborStep)
				{
					if (!isOpen || neighborFCost < neighborStep->fCost)
					{
						neighborStep->fCost = neighborFCost;
						neighborStep->parent = current;

						if (!isOpen)
						{
							open_.push_back(neighborStep);
						}
					}
				}
			}
		}
	}
}

void PathFinder::printPath(PathStep* finalStep)
{
	PathStep* current = finalStep;
	while (current->parent != nullptr)
	{
		std::cout << "(" << current->segment.second->id_ << " " << current->segment.first->position_.x << "," << current->segment.first->position_.y << "," << current->segment.first->position_.z << ")" << " ";
		current = current->parent;
	}
	std::cout << "(" << current->segment.second->id_ << " " << current->segment.first->position_.x << "," << current->segment.first->position_.y << "," << current->segment.first->position_.z << ")" << std::endl;
}
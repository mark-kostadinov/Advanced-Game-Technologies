#include "NavigationGrid.h"
#include "../../Common/Assets.h"

#include <fstream>

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE = 0;
const int RIGHT_NODE = 1;
const int TOP_NODE = 2;
const int BOTTOM_NODE = 3;

NavigationGrid::NavigationGrid()
{
	nodeSize = 0;
	gridWidth = 0;
	gridHeight = 0;
	allNodes = nullptr;
}

NavigationGrid::NavigationGrid(const std::string&filename) : NavigationGrid()
{
	std::ifstream infile(Assets::DATADIR + filename);

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	allNodes = new GridNode[gridWidth * gridHeight];

	for (int y = 0; y < gridHeight; ++y)
	{
		for (int x = 0; x < gridWidth; ++x)
		{
			GridNode&n = allNodes[(gridWidth * y) + x];
			char type = 0;
			infile >> type;
			n.type = type;
			n.position = Vector3((float)(x * gridWidth), 0, (float)(y * gridHeight));
		}
	}

	// Now to build the connectivity between the nodes
	for (int y = 0; y < gridHeight; ++y)
	{
		for (int x = 0; x < gridWidth; ++x)
		{
			GridNode&n = allNodes[(gridWidth * y) + x];

			if (y > 0)
			{ // Get the above node
				n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
			}
			if (y < gridHeight - 1)
			{ // Get the below node
				n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
			}
			if (x > 0)
			{ // Get the left node
				n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
			}
			if (x < gridWidth - 1)
			{ // Get the right node
				n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
			}
			for (int i = 0; i < 4; ++i)
			{
				if (n.connected[i])
				{
					if (n.connected[i]->type == '.')
					{
						n.costs[i] = 1;
					}
					if (n.connected[i]->type == 'p')
					{
						n.costs[i] = 1;
					}
					if (n.connected[i]->type == 'g')
					{
						n.costs[i] = 1;
					}
					if (n.connected[i]->type == 'r')
					{
						n.costs[i] = 1;
					}
					if (n.connected[i]->type == 'm')
					{
						n.costs[i] = 1;
					}
					if (n.connected[i]->type == 's') // Sandy tile, try to avoid
					{
						n.costs[i] = 3;
					}
					if (n.connected[i]->type == 'x')
					{
						n.connected[i] = nullptr; // Actually a wall, disconnect
					}
				}
			}
		}
	}
}

NavigationGrid::~NavigationGrid()
{
	delete[] allNodes;
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath)
{
	// We need to work out which node 'from' sits in, and 'to' sits in
	int fromX = (int)(from.x / nodeSize / 2.0f);
	int fromZ = (int)(from.z / nodeSize / 2.0f);

	int toX = (int)(to.x / nodeSize / 2.0f);
	int toZ = (int)(to.z / nodeSize / 2.0f);

	if (fromX < 0 || fromX > gridWidth - 1 || fromZ < 0 || fromZ > gridHeight - 1)
		return false; // Outside of map region

	if (toX < 0 || toX > gridWidth - 1 || toZ < 0 || toZ > gridHeight - 1)
		return false; // Outside of map region

	GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
	GridNode* endNode = &allNodes[(toZ * gridWidth) + toX];

	std::vector<GridNode*> openList;
	std::vector<GridNode*> closedList;

	openList.emplace_back(startNode);

	startNode->f = 0;
	startNode->g = 0;
	startNode->parent = nullptr;

	GridNode* currentBestNode = nullptr;

	while (!openList.empty())
	{
		currentBestNode = RemoveBestNode(openList);

		if (currentBestNode == endNode)
		{ // We have found the path
			GridNode* node = endNode;
			while (node != nullptr)
			{
				outPath.PushWaypoint(node->position);
				node = node->parent; // Build up the waypoints
			}
			return true;
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				GridNode* neighbour = currentBestNode->connected[i];
				if (!neighbour) // Might not be connected...
					continue;

				bool inClosed = NodeInList(neighbour, closedList);
				if (inClosed) // Already discarded this neighbour...
					continue;

				float h = Heuristic(neighbour, endNode);
				float g = currentBestNode->g + currentBestNode->costs[i];
				float f = h + g;

				bool inOpen = NodeInList(neighbour, openList);

				if (!inOpen)
					openList.emplace_back(neighbour); // It's the first time we've seen this neighbour

				// Might be a better route to this node
				if (!inOpen || f < neighbour->f)
				{
					neighbour->parent = currentBestNode;
					neighbour->f = f;
					neighbour->g = g;
				}
			}
			closedList.emplace_back(currentBestNode);
		}
	}
	return false; // Open list emptied out with no path
}

bool NavigationGrid::NodeInList(GridNode* n, std::vector<GridNode*>& list) const
{
	std::vector<GridNode*>::iterator i = std::find(list.begin(), list.end(), n);
	return i == list.end() ? false : true;
}

GridNode*  NavigationGrid::RemoveBestNode(std::vector<GridNode*>& list) const
{
	std::vector<GridNode*>::iterator bestI = list.begin();

	GridNode* bestNode = *list.begin();

	for (auto i = list.begin(); i != list.end(); ++i)
	{
		if ((*i)->f < bestNode->f)
		{
			bestNode = (*i);
			bestI = i;
		}
	}
	list.erase(bestI);
	return bestNode;
}

float NavigationGrid::Heuristic(GridNode* hNode, GridNode* endNode) const
{
	return (hNode->position - endNode->position).Length();
}
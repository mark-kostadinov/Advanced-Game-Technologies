#pragma once
#include "NavigationMap.h"
#include <string>

namespace NCL
{
	namespace CSC8503
	{
		struct GridNode
		{
			static const char OBSTACLE_NODE = 'x';
			static const char FLOOR_NODE = '.';
			static const char PLAYER_NODE = 'p';
			static const char MOVING_OBSTACLE_NODE = 'm';
			static const char GOAL_NODE = 'g';
			static const char ROBOT_NODE = 'r';
			static const char SAND_NODE = 's';

			GridNode* parent;

			GridNode* connected[4];
			int		  costs[4];

			Vector3		position;

			float f;
			float g;

			int type;

			GridNode()
			{
				for (int i = 0; i < 4; ++i)
				{
					connected[i] = nullptr;
					costs[i] = 0;
				}
				f = 0;
				g = 0;
				type = 0;
				parent = nullptr;
			}
			~GridNode() {}
		};

		class NavigationGrid : public NavigationMap
		{
		public:
			NavigationGrid();
			NavigationGrid(const std::string&filename);
			~NavigationGrid();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

			GridNode* GetNodeAt(int pos) const { return &allNodes[pos]; }
			int GetNodeSize() const { return nodeSize; }
			int GetGridSize() const { return gridWidth * gridHeight; }

		protected:
			bool		NodeInList(GridNode* n, std::vector<GridNode*>& list) const;
			GridNode*	RemoveBestNode(std::vector<GridNode*>& list) const;
			float		Heuristic(GridNode* hNode, GridNode* endNode) const;
			int nodeSize;
			int gridWidth;
			int gridHeight;

			GridNode* allNodes;
		};
	}
}


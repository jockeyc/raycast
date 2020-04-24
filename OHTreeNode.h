#ifndef OHTREENODE
#define OHTREENODE
#include <glm/glm.hpp>
enum FaceOrder : int{
	FRONT_FACE =0,
	BACK_FACE =1
};
enum class OccupancyClass : int {
	empty = 0,
	nonEmpty = 1,
	unknown = 2,
	invaid =3
};
enum class SubTreeOrder : int{
	left_top_front = 0,
	left_top_back = 1,
	left_bottom_front = 2,
	left_bottom_back = 3,
	right_top_front = 4,
	right_top_back = 5,
	right_bottom_front = 6,
	right_bottom_back = 7,
	root = 8
};
struct OHTreeNode {
public:
	OHTreeNode* children[8];
	OHTreeNode* parent;
	OccupancyClass occupancyClass;
	int occupancyHistogram[3] = { 0 };
	int depth;
	SubTreeOrder subTreeOrder;
	glm::vec3 maxPos;
	glm::vec3 minPos;


	OHTreeNode(OHTreeNode* p, SubTreeOrder order) {
		subTreeOrder = order;
		occupancyClass = OccupancyClass::unknown;
		initChildren();
		if (p != NULL) {
			parent = p;
			parent->children[(int)order] = this;
			float size = (parent->maxPos.x - parent->minPos.x) / 4;
			glm::vec3 offset = glm::vec3(0.5f - (float)(1 & ((int)order >> 2)),
				0.5f - (float)(1 & ((int)order >> 1)),
				0.5f - (float)(1 & (int)order));
			glm::vec3 parentCenter = (parent->minPos + parent->maxPos) * 0.5f;
			glm::vec3 center = parentCenter + offset * size;
			minPos = center - glm::vec3(size);
			maxPos = center + glm::vec3(size);
			depth = parent->depth + 1;	
		}
		else {
			parent = new OHTreeNode(this);
			depth = 1;
		}
	}

	OHTreeNode(OHTreeNode* root) {
		occupancyClass = OccupancyClass::invaid;
	}

	int getCount(OccupancyClass occupancyClass) {
		return occupancyHistogram[(int)occupancyClass];
	}

	void update() {
		initHistogram();
		for (int i = 0; i < 8; i++) {
			occupancyHistogram[(int)OccupancyClass::empty] += children[i]->getCount(OccupancyClass::empty);
			occupancyHistogram[(int)OccupancyClass::nonEmpty] += children[i]->getCount(OccupancyClass::nonEmpty);
			occupancyHistogram[(int)OccupancyClass::unknown] += children[i]->getCount(OccupancyClass::unknown);
		}
		int index, max = 0;
		int zeroCount = 0;
		for (int i = 0; i < 3; i++) {
			if (occupancyHistogram[i] > max) {
				index = i;
				max = occupancyHistogram[i];
			}
			if (occupancyHistogram[i] == 0)
				zeroCount++;
		}
		if (zeroCount == 2) {	//仅剩某单一属性 删除子树
			initChildren();
		}
		occupancyClass = (OccupancyClass)index;
		if (parent->occupancyClass != OccupancyClass::invaid) parent->update();
	}

	bool isLeaf() {
		return children[0] == NULL;
	}
	void initHistogram() {
		memset(occupancyHistogram, 0, sizeof(int) * 3);
	}
	void initChildren() {
		memset(children, NULL, sizeof(OHTreeNode*) * 8);
	}
};
#endif // !OHTREENODE
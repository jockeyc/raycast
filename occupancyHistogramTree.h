#ifndef OCCUPANCYHISTOGRAMTREE
#define OCCUPANCYHISTOGRAMTREE
#include "OHTreeNode.h"
class OccupancyHistogramTree {
private:
	OHTreeNode* root;
	int maxNode;
	int maxDepth;
public:
	OccupancyHistogramTree();
	void setMaxNode(int max);
	void setMaxDepth(int max);
	void setPosition(glm::vec3 min, glm::vec3 max);
	void PropagateOccupancyHistograms(OHTreeNode* node);
};

OccupancyHistogramTree::OccupancyHistogramTree() {
	root = new OHTreeNode(NULL, SubTreeOrder::root, 1);
}

void OccupancyHistogramTree::setMaxNode(int max) {
	maxNode = max;
}

void OccupancyHistogramTree::setMaxDepth(int max) {
	maxDepth = max;
}

void OccupancyHistogramTree::setPosition(glm::vec3 min, glm::vec3 max) {
	root->maxPos = max;
	root->minPos = min;
}

void OccupancyHistogramTree::PropagateOccupancyHistograms(OHTreeNode* node) {
	node->initHistogram();
	for (int i = 0; i < 8; i++) {
		PropagateOccupancyHistograms(node->children[i]);
	}
	for (int i = 0; i < 8; i++) {
		node->occupancyHistogram[(int)OccupancyClass::empty] += node->children[i]->getCount(OccupancyClass::empty);
		node->occupancyHistogram[(int)OccupancyClass::nonEmpty] += node->children[i]->getCount(OccupancyClass::nonEmpty);
		node->occupancyHistogram[(int)OccupancyClass::unknown] += node->children[i]->getCount(OccupancyClass::unknown);
	}
	int index, max = 0;
	int zeroCount = 0;
	for (int i = 0; i < 3; i++) {
		if (node->occupancyHistogram[i] > max) {
			index = i;
			max = node->occupancyHistogram[i];
		}
		if (node->occupancyHistogram[i] == 0)
			zeroCount++;
	}
	if (zeroCount == 2) {	//仅剩某单一属性 删除子树
		memset(node->children, NULL, sizeof(OHTreeNode*) * 8);
	}
	node->occupancyClass = (OccupancyClass)index;

}
#endif // !OCCUPANCYHISTOGRAMTREE

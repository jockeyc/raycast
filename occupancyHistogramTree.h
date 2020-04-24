#ifndef OCCUPANCYHISTOGRAMTREE
#define OCCUPANCYHISTOGRAMTREE
#include "OHTreeNode.h"
#include "NodeInfo.h"
#include <queue>
#include <map>
#include <vector>
class OccupancyHistogramTree {
private:
	OHTreeNode* root;
	int maxBoundingBox;
	int maxDepth;
	unsigned char* volumeData;
	glm::vec3 CameraPos;
	std::vector<NodeInfo*> OccupancyGeometryArray;
	
	//int traversalCount;
public:

	std::vector<NodeInfo*> OccupancyIndexArray;
	OccupancyHistogramTree();
	void setMaxNode(int max);
	void setMaxDepth(int max);
	void setPosition(glm::vec3 min, glm::vec3 max);
	void setCameraPos(glm::vec3 pos);
	void setVolumeData(unsigned char* data);
	void PropagateOccupancyHistograms(OHTreeNode* node);
	void Propagation();
	void TraversalOccupancyGeometryGeneration();
	void emitGeometry(OHTreeNode* node);
	void emitIndex(OHTreeNode* node, FaceOrder face);
	void TraversalIndexOrder(OHTreeNode* node);
	void TraversalIndexOrderRoot();
	void getSubOrder(OHTreeNode* node, std::vector<SubTreeOrder>& vec);
	//bool static subOrderCompare(std::pair<SubTreeOrder, glm::vec3>& A, std::pair<SubTreeOrder, glm::vec3>& B);
	void subDivision(OHTreeNode* node);
	void subDivisionRoot();
	void sampleVolume(OHTreeNode* node);
};

OccupancyHistogramTree::OccupancyHistogramTree() {
	root = new OHTreeNode(NULL, SubTreeOrder::root);
	maxBoundingBox = 1;
	maxDepth = 1;
	volumeData = NULL;
}

void OccupancyHistogramTree::setMaxNode(int max) {
	maxBoundingBox = max;
}

void OccupancyHistogramTree::setMaxDepth(int max) {
	maxDepth = max;
}

void OccupancyHistogramTree::setPosition(glm::vec3 min, glm::vec3 max) {
	root->maxPos = max;
	root->minPos = min;
}

void OccupancyHistogramTree::setVolumeData(unsigned char* data) {
	volumeData = data;
}

void OccupancyHistogramTree::setCameraPos(glm::vec3 pos){
	CameraPos = pos;
}

void OccupancyHistogramTree::PropagateOccupancyHistograms(OHTreeNode* node) {
	node->initHistogram();
	if (node->isLeaf()) {
		node->occupancyHistogram[(int)node->occupancyClass] = 1;
		return;
	}
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

void OccupancyHistogramTree::Propagation() {
	PropagateOccupancyHistograms(root);
}

void OccupancyHistogramTree::TraversalOccupancyGeometryGeneration() {
	std::queue<OHTreeNode*> queue;
	queue.push(root);
	int boxCount = 0;
	while (!queue.empty() && boxCount <= maxBoundingBox) {
		OHTreeNode* node = queue.front();
		queue.pop();
		if (node->parent) {
			if (node->occupancyClass != node->parent->occupancyClass) {
				emitGeometry(node);
				emitGeometry(node->parent);
			}
		}
		else emitGeometry(node);
		for (int i = 0; i < 8; i++) {
			queue.push(node->children[i]);
		}
		boxCount++;
	}
}

void OccupancyHistogramTree::emitGeometry(OHTreeNode* node) {
}

void OccupancyHistogramTree::TraversalIndexOrder(OHTreeNode* node) {
	
	if (node->isLeaf()) {
		if (node->occupancyClass != node->parent->occupancyClass) {
			emitIndex(node, FaceOrder::FRONT_FACE);
			emitIndex(node, FaceOrder::BACK_FACE);
		}
	}
	else {
		if (node->occupancyClass != node->parent->occupancyClass) {
			emitIndex(node, FaceOrder::FRONT_FACE);
		}
		std::vector<SubTreeOrder> subOrder;
		getSubOrder(node, subOrder);
		for (int i = 0; i < 8; i++) {
			TraversalIndexOrder(node->children[(int)subOrder[i]]);
		}
		if (node->occupancyClass != node->parent->occupancyClass) {
			emitIndex(node, FaceOrder::BACK_FACE);
		}
	}
}

void OccupancyHistogramTree::TraversalIndexOrderRoot() {
	OccupancyIndexArray.clear();
	TraversalIndexOrder(root);
}

void OccupancyHistogramTree::emitIndex(OHTreeNode* node, FaceOrder face) {
	OccupancyIndexArray.push_back(new NodeInfo(node,face));
}

bool subOrderCompare(std::pair<SubTreeOrder, float>& A, std::pair<SubTreeOrder, float>& B) {
	float DisA = A.second;
	float DisB = B.second;
	return DisA < DisB;
}

void OccupancyHistogramTree::getSubOrder(OHTreeNode* node, std::vector<SubTreeOrder>& vec)
{
	std::map<SubTreeOrder,float> subCenter;
	for (int i = 0; i < 8; i++) {
		float distance = glm::distance((node->minPos + node->maxPos) * 0.5f, CameraPos);
		subCenter.insert(std::pair<SubTreeOrder, float>((SubTreeOrder)i,distance));
	}
	std::vector< std::pair<SubTreeOrder, float>> vecCompare(subCenter.begin(),subCenter.end());
	std::sort(vecCompare.begin(), vecCompare.end(), subOrderCompare);
	for (int i = 0; i < 8; i++) {
		vec.push_back(vecCompare[i].first);
	}
}

void  OccupancyHistogramTree::subDivision(OHTreeNode* node) {
	if (node->depth == maxDepth) {
		sampleVolume(node);
	}
	else {
		for (int i = 0; i < 8; i++) {
			node->children[i] = new OHTreeNode(node, (SubTreeOrder)i);
			subDivision(node->children[i]);
		}
		//node->update();
	}
}

void OccupancyHistogramTree::subDivisionRoot() {
	subDivision(root);
}

void OccupancyHistogramTree::sampleVolume(OHTreeNode* node) {
	int width = 128;
	int height = 128;
	int depth = 128;

	int size = (int)std::pow(2, maxDepth - 1);
	glm::vec3 pos = (node->minPos + node->maxPos) * 0.5f;
	pos = pos - root->minPos;
	float x = pos.x / (root->maxPos.x - root->minPos.x);
	float y = pos.y / (root->maxPos.y - root->minPos.y);
	float z = pos.z / (root->maxPos.z - root->minPos.z);
	int w = x * width,
		h = y * height,
		d = z * depth;
	int index = d * width * height + h * width + w;
	if (volumeData[index] < 25) {
		node->occupancyClass = OccupancyClass::empty;
	}
	else node->occupancyClass = OccupancyClass::nonEmpty;	
}
#endif // !OCCUPANCYHISTOGRAMTREE


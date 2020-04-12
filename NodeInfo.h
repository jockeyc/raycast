#include "OHTreeNode.h"
#pragma once
struct NodeInfo
{
	OHTreeNode* node;
	FaceOrder faceOrder;
	NodeInfo(OHTreeNode* n, FaceOrder order) {
		node = n;
		faceOrder = order;
	}
};
#pragma once
#include "../External/fkernel/Include/DataStructure/FSurface.h"
#include <vector>

using namespace std;

class FInsFeature;

class SurfNearbyGraph_Node
{
public:
	SurfNearbyGraph_Node(int paraFSurfIndex) :m_SurfIndex(paraFSurfIndex) {};
	~SurfNearbyGraph_Node() {};

	int GetGeomIndex() { return m_SurfIndex; };

	vector<SurfNearbyGraph_Node*>& GetNearbyNodeList() { return m_NearbyNode; };

	// ≈–∂œ this ”Î other_node  «∑Òœ‡¡⁄
	bool IsNearby(SurfNearbyGraph_Node* other_node);


private:
	int m_SurfIndex;
	
	vector<SurfNearbyGraph_Node*> m_NearbyNode;

};

class InsFeaNearbyGraph_Node
{
public:
	InsFeaNearbyGraph_Node(int paraSurfInd) :m_SurfIndex(paraSurfInd) {};
	~InsFeaNearbyGraph_Node() {};



private:
	int m_SurfIndex;

	vector<FInsFeature*> m_NearInsFea;

};


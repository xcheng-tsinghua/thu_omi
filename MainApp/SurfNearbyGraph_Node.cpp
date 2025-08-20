#include "SurfNearbyGraph_Node.h"

bool SurfNearbyGraph_Node::IsNearby(SurfNearbyGraph_Node* other_node)
{
	for (auto ita = other_node->GetNearbyNodeList().begin(); ita != other_node->GetNearbyNodeList().end(); ++ita)
	{
		if ((*ita)->GetGeomIndex() == m_SurfIndex)
		{
			return true;
		}
	}
	return false;
}

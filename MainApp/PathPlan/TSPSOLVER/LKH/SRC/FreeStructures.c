#include "LKH.h"
#include "Sequence.h"
#include "Genetic.h"

/*      
 * The FreeStructures function frees all allocated structures.
 */

#define TSP_Free(s) { free(s); s = 0; }

void FreeStructures()
{
    FreeCandidateSets();
    FreeSegments();
    if (NodeSet) {
        int i;
        for (i = 1; i <= Dimension; i++) {
            Node *N = &NodeSet[i];
            TSP_Free(N->MergeSuc);
            N->C = 0;
        }
        TSP_Free(NodeSet);
    }
    TSP_Free(CostMatrix);
    TSP_Free(BestTour);
    TSP_Free(BetterTour);
    TSP_Free(SwapStack);
    TSP_Free(HTable);
    TSP_Free(Rand);
    TSP_Free(CacheSig);
    TSP_Free(CacheVal);
    TSP_Free(Name);
    TSP_Free(Type);
    TSP_Free(EdgeWeightType);
    TSP_Free(EdgeWeightFormat);
    TSP_Free(EdgeDataFormat);
    TSP_Free(NodeCoordType);
    TSP_Free(DisplayDataType);
    TSP_Free(Heap);
    TSP_Free(t);
    TSP_Free(T);
    TSP_Free(tSaved);
    TSP_Free(p);
    TSP_Free(q);
    TSP_Free(incl);
    TSP_Free(cycle);
    TSP_Free(G);
    FreePopulation();
}

/*      
   The FreeSegments function frees the segments.
 */

void FreeSegments()
{
    if (FirstSegment) {
        Segment *S = FirstSegment, *SPrev;
        do {
            SPrev = S->Pred;
            TSP_Free(S);
        }
        while ((S = SPrev) != FirstSegment);
        FirstSegment = 0;
    }
    if (FirstSSegment) {
        SSegment *SS = FirstSSegment, *SSPrev;
        do {
            SSPrev = SS->Pred;
            TSP_Free(SS);
        }
        while ((SS = SSPrev) != FirstSSegment);
        FirstSSegment = 0;
    }
}

/*      
 * The FreeCandidateSets function frees the candidate sets.
 */

void FreeCandidateSets()
{
    Node *N = FirstNode;
    if (!N)
        return;
    do {
        TSP_Free(N->CandidateSet);
        TSP_Free(N->BackboneCandidateSet);
    }
    while ((N = N->Suc) != FirstNode);
}

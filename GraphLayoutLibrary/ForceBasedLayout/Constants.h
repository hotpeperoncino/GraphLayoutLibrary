#ifndef LAYOUTCONSTANTS_H
#define LAYOUTCONSTANTS_H

// Contains constants used in Force directed algorithm.

#define ITERATIONS 100                               /*!< fallback iteration count  */
#define DEFAULT_EDGE_LENGTH 50                      /*!< desired edge length */
#define ATTRACTION_MULTIPLIER 0.5                   /*!< default attraction multiplier */
#define REPULSION_RANGE 1000000                     /*!< max repulsion range */
#define REPULSION_MULTIPLIER 100                    /*!< default repulsion multiplier */
#define CLUSTER_REPULSION_RANGE 10000               /*!< max cluster repulsion range */
#define CLUSTER_REPULSION_MULTIPLIER 10000          /*!< cluster-cluster repulsion multiplier */
#define CLUSTER_TO_CLUSTER_REPULSION_RANGE 10000    /*!< cluster replusion range */
#define GLOBAL_ID 999
#define UNIT_DISPLACEMENT 10.0                      /*!< unit displacement */
#define CLOSE_TO_BOUNDRY 200                        /*!< boundry nearness condition */
#define UNIT_CLUSTER_DISPLACEMENT 15.0              /*!< unit cluster displacement */
#define CLUSTER_OVERLAP_BOUNDRY 150                 /*!< cluster nearness condition */
#define CORRECTED_DISTANCE 100                      /*!< overlap correction */
#define CLUSTER_OVERLAP_REMOVAL_PASSES 7            /*!< cluster overlap removal passes */
#define VERTEX_OVERLAP_REMOVAL_PASSES 5             /*!< vertex overlap removal passes */
#define INTEREDGES_DEFAULT_LENGTH 100               /*!< desired inter edge length */
#define INTEREDGES_ATTRACTION_MULTIPLIER 0.3        /*!< default inter edge attraction multiplier */
#define MINIMUM_INTEREDGE_DISTANCE 250              /*!< inter edge dist */


#endif // LAYOUTCONSTANTS_H

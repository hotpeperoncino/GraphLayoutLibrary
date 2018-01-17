#include "CircleLayouter.h"

CircleLayouter::CircleLayouter()
{    
    m_iDefaultHeight = 40;
    m_iDefaultWidth = 80;
}

double CircleLayouter::calculateAngleSpace(VertexDescriptor& vVertex, SubGraph &gSubgraph)
{

    LAYOUT_ASSERT(&gSubgraph != NULL, LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, NULL_GRAPH_FOUND));

    double dAngleSpace = 0;
    double dCircumference;
    // calculate circumference in terms of diagonal of nodes
    try
    {
        dCircumference = calculateCircumference(gSubgraph);
    }
    catch(LayoutException& eException)
    {
        QString sGraphId = m_boostGraphWrapper.getGraphId(gSubgraph);
        qDebug()<<"Default : Circumference is calculated"<<sGraphId;
        int iTotalVertexCount = num_vertices(gSubgraph);
        // calculate the default height and default width
        double dDefaultDiagonalLengthSquare = (double)((m_iDefaultHeight * m_iDefaultHeight) +
                                                       (m_iDefaultWidth * m_iDefaultWidth));

        double dDefaultDiagonalLength = (double)qSqrt(dDefaultDiagonalLengthSquare);
        dCircumference = (double)iTotalVertexCount * dDefaultDiagonalLength;
    }
    catch(LayoutMemoryException& eException)
    {
        throw LayoutMemoryException(__FUNCTION__,LayoutExceptionEnum::NULL_POINTER_EXCEPTION,eException.getObjectName());
    }
    catch(boost::exception& eBoostException)
    {
        throw *boost::get_error_info<errmsg_info>(eBoostException);
    }

    // calculate the diagonal length of this node
    SizeManager sizeManager;
    double dNodeDiagonalLength;
    try
    {
        dNodeDiagonalLength = sizeManager.calculateDiagonalOfNode(vVertex, gSubgraph);
    }
    catch(boost::exception& )
    {
        QString sVertexId = m_boostGraphWrapper.getVertexId(vVertex, gSubgraph);
        qDebug()<<"Default diagonal length caluclated for vertex"<<sVertexId;
        double dDefaultDiagonalLengthSquare = (double)((m_iDefaultHeight * m_iDefaultHeight) +
                                                       (m_iDefaultWidth * m_iDefaultWidth));
        dNodeDiagonalLength = (double)qSqrt(dDefaultDiagonalLengthSquare);
    }
    catch(...)
    {
        QString sVertexId = m_boostGraphWrapper.getVertexId(vVertex, gSubgraph);
        qDebug()<<"Default diagonal length caluclated for vertex"<<sVertexId;
        double dDefaultDiagonalLengthSquare = (double)((m_iDefaultHeight * m_iDefaultHeight) +
                                                       (m_iDefaultWidth * m_iDefaultWidth));
        dNodeDiagonalLength = (double)qSqrt(dDefaultDiagonalLengthSquare);
    }

    /*
     * This is the factor which calculates the proportion of node length
       in terms of diagonal and total circumference calculated in terms
       of adding all nodes diagonal length.
    */
    double dNodeDiagonalWithRadiusShare = dNodeDiagonalLength; //+ (0.0009381 * RADIUS_SHARE);

//    int iTotalVertices = num_vertices(gSubgraph);
//    int iDummyVertices = gSubgraph.num_children();
//    int iVertexCount = iTotalVertices - iDummyVertices;
    double dCircumferenceWithShare = dCircumference; //+ (2 * (m_dPiValue) * RADIUS_SHARE);
    double dProportionFactor = dNodeDiagonalWithRadiusShare / dCircumferenceWithShare;

    // Calculate the  angle space required for this node
    dAngleSpace = ((2 * (PI)) * dProportionFactor);

    return dAngleSpace;
}


double CircleLayouter::calculateCircumference(SubGraph &gSubgraph)
{

    LAYOUT_ASSERT(&gSubgraph != NULL, LayoutMemoryException(__FUNCTION__, LayoutExceptionEnum::NULL_POINTER_EXCEPTION, NULL_GRAPH_FOUND));

    int iCircumference = 0;
    int iNumVertices = num_vertices(gSubgraph);
    if(iNumVertices != 0)
    {
        SizeManager sizeManager;
        BGL_FORALL_VERTICES(vVertex, gSubgraph, SubGraph)
        {

            LayoutEnum::NodeType nodeType = m_boostGraphWrapper.getVertexType(vVertex,gSubgraph);
            if(LayoutEnum::isValidCircularVertexType(nodeType) == true)
            {
                // check whether the node is valid node within available types
                if(nodeType == LayoutEnum::DummyNode)
                {
                    // Skip Dummy Nodes
                    continue;
                }
                double iNodeDiagonalLength;
                try
                {
                    iNodeDiagonalLength = sizeManager.calculateDiagonalOfNode(vVertex, gSubgraph);
                    ////cout<<"Diagonal of Node : "<<vVertex<<" "<<iNodeDiagonalLength<<endl;
                    iCircumference += iNodeDiagonalLength;
                }
                catch(LayoutMemoryException& eException)
                {
                    throw LayoutMemoryException(__FUNCTION__,
                                          LayoutExceptionEnum::NULL_POINTER_EXCEPTION,
                                          eException.getObjectName());
                }
                catch(boost::exception& eBoostException)
                {
                    throw *boost::get_error_info<errmsg_info>(eBoostException);
                }
            }
            else
            {
                //cout<<"Invalid Node Type for Circular Layout"<<endl;
                throw LayoutException(__FUNCTION__, LayoutExceptionEnum::INVALID_PARAMETER, INVALID_NODE_TYPE);
            }
        }
    }
    else
    {
        int iHeight = m_boostGraphWrapper.getGraphHeight(gSubgraph);
        int iWidth = m_boostGraphWrapper.getGraphWidth(gSubgraph);
        int iGraphDiagonalSquare = (iHeight * iHeight) + (iWidth * iWidth);
        iCircumference = qSqrt(iGraphDiagonalSquare);
    }
    return iCircumference;
}

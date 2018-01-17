#include "LayoutEnum.h"

LayoutEnum::LayoutEnum()
{
}

bool LayoutEnum::isValidVertexOrderingCriteria(LayoutEnum::VertexOrderCriteria enVertexOrderCriteria)
{
    bool isValidOrderCriteria = false;

    switch(enVertexOrderCriteria)
    {
    case LayoutEnum::TopologicalOrder:
        isValidOrderCriteria = true;
        break;

    case LayoutEnum::ConnectedComponentOrder:
        isValidOrderCriteria = true;
        break;

    case LayoutEnum::DefaultOrder:
        isValidOrderCriteria = true;
        break;
    }
    return isValidOrderCriteria;
}

bool LayoutEnum::isValidCircularVertexType(NodeType enVertexType)
{
    bool bIsValidCircularVertexType = false;

    switch(enVertexType)
    {
    case LayoutEnum::DummyNode:
        bIsValidCircularVertexType = true;
        break;

    case LayoutEnum::GraphNode:
        bIsValidCircularVertexType = true;
        break;

    case LayoutEnum::InvisibleNode:
        bIsValidCircularVertexType = true;
        break;
    }
    return bIsValidCircularVertexType;
}

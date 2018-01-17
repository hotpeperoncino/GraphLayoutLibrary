#ifndef LAYOUTTYPES_H
#define LAYOUTTYPES_H

/**
 * Enum for type of layout algorithm
 */
enum LayoutType{
    CircularLayout,
    HierarchicalLayout,
    GridBasedLayout,
    ForceDirectedLayout,
    RandomLayout
    
};

//bool isValidLayoutType(const LayoutType& enLayoutType)
//{
//    bool bIsValidLayoutType = false;

//    if(CircularLayout <= enLayoutType
//       &&  enLayoutType <= HierarchicalLayout)
//    {
//        bIsValidLayoutType =true;
//    }

//    return bIsValidLayoutType;
//}


#endif // LAYOUTTYPES_H

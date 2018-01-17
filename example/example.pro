#-------------------------------------------------
#
# Project created by QtCreator 2013-09-10T12:17:10
#
#-------------------------------------------------

QT       -= gui
QT += xml xmlpatterns

CONFIG += c++11

CONFIG += exceptions
#CONFIG+= staticlib


TARGET = a.out
TEMPLATE = app

DEFINES += 

SOURCES += example.cpp


HEADERS += \
    ../GraphLayoutLibrary/GraphLayoutLibrary.h \
    ../GraphLayoutLibrary/GraphLayoutLibrary_global.h \
    ../GraphLayoutLibrary/GraphMLReader/GraphMLReader.h \
    ../GraphLayoutLibrary/GraphMLReader/dirtyedge.h \
    ../GraphLayoutLibrary/GraphMLReader/dirtynode.h \
    ../GraphLayoutLibrary/GraphMLWriter/GraphMLWriter.h \
    ../GraphLayoutLibrary/ReaderWriterUtilities/AttributeConstants.h \
    ../GraphLayoutLibrary/ReaderWriterUtilities/Attribute.h \
    ../GraphLayoutLibrary/ReaderWriterUtilities/DefaultValues.h \
    ../GraphLayoutLibrary/ReaderWriterUtilities/GraphElementProperties.h \
    ../GraphLayoutLibrary/ReaderWriterUtilities/Typedefs.h \
    ../GraphLayoutLibrary/Common/GraphType.h \
    ../GraphLayoutLibrary/Common/BoostGraphWrapper.h \
    ../GraphLayoutLibrary/LayoutUtilities/LayoutTypes.h \
    ../GraphLayoutLibrary/LayoutUtilities/CommonLayoutConstants.h \
    ../GraphLayoutLibrary/LayoutManager/LayoutManager.h \
    ../GraphLayoutLibrary/CircularLayout/CircularLayoutGenerator.h \
    ../GraphLayoutLibrary/Common/LayoutUtility.h \
    ../GraphLayoutLibrary/RandomLayout/RandomLayoutGenerator.h \
    ../GraphLayoutLibrary/Common/GeometryHelper.h \
    ../GraphLayoutLibrary/CircularLayout/CircleLayouter.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/HierarchicalLayouter.h \
    ../GraphLayoutLibrary/Common/GraphCycleHandler.h \
    ../GraphLayoutLibrary/CircularLayout/SpaceUtilizer.h \
    ../GraphLayoutLibrary/Common/CustomDFSVisitors.h \
    ../GraphLayoutLibrary/Common/CustomBFSVisitors.h \
    ../GraphLayoutLibrary/GraphMLReader/MessageHandler.h \
    ../GraphLayoutLibrary/CircularLayout/SizeManager.h \
    ../GraphLayoutLibrary/LayoutException/LayoutException.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/LayerNode.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h \
    ../GraphLayoutLibrary/Common/ConstantType.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/HierarchicalLayoutTestingConstants.h \
    ../GraphLayoutLibrary/Common/CommonConstants.h \
    ../GraphLayoutLibrary/ForceBasedLayout/ForceBasedLayoutGenerator.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/HierarchicalLayoutTypedefs.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/ReducedNestingTreeNode.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/SubgraphOrderingGraphType.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/SubgraphOrderingGraphDFSVisitor.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/SubgraphOrderingGraph.h \
    ../GraphLayoutLibrary/LayoutManager/GraphLayoutErrorCodes.h \
    ../GraphLayoutLibrary/ForceBasedLayout/ForceIterator.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/HierarchicalLayoutGenerator.h \
    ../GraphLayoutLibrary/ForceBasedLayout/ForceDirectedLayouter.h \
    ../GraphLayoutLibrary/ForceBasedLayout/Reingold.h \
    ../GraphLayoutLibrary/ForceBasedLayout/MembershipInitializer.h \
    ../GraphLayoutLibrary/HierarchicalLayoutGenerator/LayersSubgraphSorter.h \
    ../GraphLayoutLibrary/GridBasedLayout/GridBasedLayout.h \
    ../GraphLayoutLibrary/GridBasedLayout/GraphPreProcessor.h \
    ../GraphLayoutLibrary/ForceBasedLayout/BeautifyLayout.h \
    ../GraphLayoutLibrary/ForceBasedLayout/SpringEmbedder.h \
    ../GraphLayoutLibrary/GridBasedLayout/GraphPreProcessor.h \
    ../GraphLayoutLibrary/GridBasedLayout/CreateDualGraph.hpp \
    ../GraphLayoutLibrary/GridBasedLayout/MyPlanarFaceTraversal.hpp \
    ../GraphLayoutLibrary/ForceBasedLayout/ClusteredSpringEmbedder.h \
    ../GraphLayoutLibrary/ForceBasedLayout/Postprocessing.h \
    ../GraphLayoutLibrary/GridBasedLayout/GridLayouter.h \
    ../GraphLayoutLibrary/ForceBasedLayout/RelaxInterEdges.h \
    ../GraphLayoutLibrary/ForceBasedLayout/Constants.h \
    ../GraphLayoutLibrary/Common/LayoutEnum.h \
    ../GraphLayoutLibrary/LayoutException/LayoutExceptionConstants.h \
    ../GraphLayoutLibrary/LayoutException/LayoutExceptionEnum.h \
    ../GraphLayoutLibrary/LayoutException/LayoutFileIOException.h \
    ../GraphLayoutLibrary/LayoutException/LayoutMemoryException.h \
    ../GraphLayoutLibrary/ForceBasedLayout/VertexOverlapRemoval.h

unix:!symbian {
    target.path = .
    INSTALLS += target
}

LIBS=../GraphLayoutLibrary/libGraphLayoutLibrary.so

INCLUDEPATH += $$PWD
INCLUDEPATH += ../GraphLayoutLibrary
#INCLUDEPATH += E:/boost_1_53_0/boost_1_53_0
#INCLUDEPATH += E:/boost_1_46_1/boost_1_46_1
#INCLUDEPATH += E:/boost_1_52_0/boost_1_52_0
#INCLUDEPATH += E:/Softwares/SCoMoS_Softwares/boost_1_55_0/boost_1_55_0

RESOURCES += 

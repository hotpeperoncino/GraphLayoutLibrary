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

TARGET = GraphLayoutLibrary
TEMPLATE = lib

DEFINES += GRAPHLAYOUTLIBRARY_LIBRARY

SOURCES += GraphLayoutLibrary.cpp \    
    GraphMLReader/GraphMLReader.cpp \
    GraphMLReader/dirtynode.cpp \
    GraphMLReader/dirtyedge.cpp \
    GraphMLWriter/GraphMLWriter.cpp \
    LayoutManager/LayoutManager.cpp \
    CircularLayout/CircularLayoutGenerator.cpp \
    Common/BoostGraphWrapper.cpp \
    Common/LayoutUtility.cpp \
    RandomLayout/RandomLayoutGenerator.cpp \
    Common/GeometryHelper.cpp \
    CircularLayout/CircleLayouter.cpp \
    HierarchicalLayoutGenerator/HierarchicalLayouter.cpp \
    CircularLayout/SpaceUtilizer.cpp \
    Common/GraphCycleHandler.cpp \
    CircularLayout/SizeManager.cpp \
    LayoutException/LayoutException.cpp \
    HierarchicalLayoutGenerator/LayerNode.cpp \
    HierarchicalLayoutGenerator/NestingTreeSubgraphNode.cpp \
    ForceBasedLayout/ForceBasedLayoutGenerator.cpp \
    HierarchicalLayoutGenerator/ReducedNestingTreeNode.cpp \
    HierarchicalLayoutGenerator/SubgraphOrderingGraph.cpp \
    LayoutManager/GraphLayoutErrorCodes.cpp \
    HierarchicalLayoutGenerator/HierarchicalLayoutGenerator.cpp \
    ForceBasedLayout/Reingold.cpp \
    HierarchicalLayoutGenerator/LayersSubgraphSorter.cpp \
    ForceBasedLayout/MembershipInitializer.cpp \
    GridBasedLayout/GraphPreProcessor.cpp \
    GridBasedLayout/GridBasedLayout.cpp \
    ForceBasedLayout/SpringEmbedder.cpp \
    ForceBasedLayout/ClusteredSpringEmbedder.cpp \
    ForceBasedLayout/postprocessing.cpp \
    GridBasedLayout/GridLayouter.cpp \
    ForceBasedLayout/RelaxInterEdges.cpp \
    Common/LayoutEnum.cpp \
    LayoutException/LayoutExceptionEnum.cpp \
    LayoutException/LayoutFileIOException.cpp \
    LayoutException/LayoutMemoryException.cpp \
    ForceBasedLayout/VertexOverlapRemoval.cpp


HEADERS += GraphLayoutLibrary.h\
        GraphLayoutLibrary_global.h \
    GraphMLReader/GraphMLReader.h \
    GraphMLReader/dirtyedge.h \
    GraphMLReader/dirtynode.h \
    GraphMLWriter/GraphMLWriter.h \
    ReaderWriterUtilities/AttributeConstants.h \
    ReaderWriterUtilities/Attribute.h \
    ReaderWriterUtilities/DefaultValues.h \
    ReaderWriterUtilities/GraphElementProperties.h \
    ReaderWriterUtilities/Typedefs.h \
    Common/GraphType.h \
    Common/BoostGraphWrapper.h \
    LayoutUtilities/LayoutTypes.h \
    LayoutUtilities/CommonLayoutConstants.h \
    LayoutManager/LayoutManager.h \
    CircularLayout/CircularLayoutGenerator.h \
    Common/LayoutUtility.h \
    RandomLayout/RandomLayoutGenerator.h \
    Common/GeometryHelper.h \
    CircularLayout/CircleLayouter.h \
    HierarchicalLayoutGenerator/HierarchicalLayouter.h \
    Common/GraphCycleHandler.h \
    CircularLayout/SpaceUtilizer.h \
    Common/CustomDFSVisitors.h \
    Common/CustomBFSVisitors.h \
    GraphMLReader/MessageHandler.h \
    CircularLayout/SizeManager.h \
    LayoutException/LayoutException.h \
    HierarchicalLayoutGenerator/LayerNode.h \
    HierarchicalLayoutGenerator/NestingTreeSubgraphNode.h \
    Common/ConstantType.h \
    HierarchicalLayoutGenerator/HierarchicalLayoutTestingConstants.h \
    Common/CommonConstants.h \
    ForceBasedLayout/ForceBasedLayoutGenerator.h \
    HierarchicalLayoutGenerator/HierarchicalLayoutTypedefs.h \
    HierarchicalLayoutGenerator/ReducedNestingTreeNode.h \
    HierarchicalLayoutGenerator/SubgraphOrderingGraphType.h \
    HierarchicalLayoutGenerator/SubgraphOrderingGraphDFSVisitor.h \
    HierarchicalLayoutGenerator/SubgraphOrderingGraph.h \
    LayoutManager/GraphLayoutErrorCodes.h \
    ForceBasedLayout/ForceIterator.h \
    HierarchicalLayoutGenerator/HierarchicalLayoutGenerator.h \
    ForceBasedLayout/ForceDirectedLayouter.h \
    ForceBasedLayout/Reingold.h \
    ForceBasedLayout/MembershipInitializer.h \
    HierarchicalLayoutGenerator/LayersSubgraphSorter.h \
    GridBasedLayout/GridBasedLayout.h \
    GridBasedLayout/GraphPreProcessor.h \
    ForceBasedLayout/BeautifyLayout.h \
    ForceBasedLayout/SpringEmbedder.h \
    GridBasedLayout/GraphPreProcessor.h \
    GridBasedLayout/CreateDualGraph.hpp \
    GridBasedLayout/MyPlanarFaceTraversal.hpp \
    ForceBasedLayout/ClusteredSpringEmbedder.h \
    ForceBasedLayout/Postprocessing.h \
    GridBasedLayout/GridLayouter.h \
    ForceBasedLayout/RelaxInterEdges.h \
    ForceBasedLayout/Constants.h \
    Common/LayoutEnum.h \
    LayoutException/LayoutExceptionConstants.h \
    LayoutException/LayoutExceptionEnum.h \
    LayoutException/LayoutFileIOException.h \
    LayoutException/LayoutMemoryException.h \
    ForceBasedLayout/VertexOverlapRemoval.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1CBFA86
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = GraphLayoutLibrary.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


INCLUDEPATH += $$PWD
#INCLUDEPATH += E:/boost_1_53_0/boost_1_53_0
#INCLUDEPATH += E:/boost_1_46_1/boost_1_46_1
#INCLUDEPATH += E:/boost_1_52_0/boost_1_52_0
#INCLUDEPATH += E:/Softwares/SCoMoS_Softwares/boost_1_55_0/boost_1_55_0

RESOURCES += \
    LayoutResources/graphMLSchema.qrc

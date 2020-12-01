#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LDCK4CCE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LDCK4CCE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LDCK4CCE_EXPORTS
#define LDCK4CCE_API __declspec(dllexport)
#else
#define LDCK4CCE_API __declspec(dllimport)
#endif

#include "../obj/tisysbak.h"   

#include "../sys/alg/isysparallel.h"
#include "../obj/ialgmesh/ialgmesh.h"
#include "../sys/alg/isysalgwxj.h" 

//#include "../sys/trn50/isyscuttrans4to5.h" 
//#include "../sys/dntl/isysdntl.h"  

//#include "../sys/alg/isysdsgn.h"
//#include "../sys/trn50/isysprdct.h"
//#include "../sys/trn50/isyscut.h"
//#include "../sys/alg/isysalgshapejaw.h"
//#include "../sys/alg/isysmeshshapefactory.h" 
//#include "../obj/alg/ialgck.h"
//
//namespace VNTrnCutTrans_5
//{
//    VD_PERSISTCLASS_BEGIN( VSJawCreatorDep)
//        VSESS< VNCollisionTest::VRMergeCollisionTest >  esCollision ;
//        VSESS< VNALGMESH::VRRefineTooth2             >  esRefine    ;
//        VSESS< VNALGMESH::VRSliceMeshBoundary        >  esSliceHole ;
//        VSESS< VNALGMESH::VRPerfGraph                >  esPerfGraph ;
//        VSESS< VNALGMESH::VRGeodicMesh               >  esGeodic    ;
//        VSESS< VNALGDSGN::VRPatchTooth               >  esPatch     ; 
//        VSESS< VNALGMESH::VRLapComb                  >  esLapComb   ;
//        VSESS< VNALGMESH::VRDijkstra                 >  esDijkstra  ; 
//        VSESS< VNALGMESH::VRLaplaceMeshEdit          >  esReshape   ;
//        VSESS< VNALGMESH::VRFindHole2                >  esFindHole   ;
//    VD_PERSISTCLASS_END()
//
//    VD_DEFRELATION( VRCutJawCreator, VSJawCreatorDep, VNTrnCut::VSJawCreator ) ;
//
//    struct VSProductorCreatorEnv 
//    {
//        VSESS< VNCollisionTest::VRMergeCollisionTest >  esCollision  ;
//        VSESS< VNALGMESH::VRRefineTooth2             >  esRefine     ;
//        VSESS< VNALGMESH::VRSliceMeshBoundary        >  esSliceHole  ;
//        VSESS< VNALGMESH::VRPerfGraph                >  esPerfGraph  ;
//        VSESS< VNALGMESH::VRGeodicMesh               >  esGeodic     ;
//        VSESS< VNALGDSGN::VRPatchTooth               >  esPatch      ; 
//        VSESS< VNALGMESH::VRLapComb                  >  esLapComb    ;
//        VSESS< VNALGMESH::VRDijkstra                 >  esDijkstra   ;
//        VSESS< VNALGMESH::VRFindHole2                >  esFindHole   ;
//        VSESS< VNALGMESH::VRVtMap2SphereCoord        >  esL2CMapper  ;
//        VSESS< VNALGMESH::VRSphereCoordMap2Vt        >  esC2Lmapper  ;
//        VSESS< VNALGMESH::VRMLSInterpCircle          >  esInterpCrcl ;
//        VSESS< VNALGMESH::VRMLSInterp                >  esInterp     ;
//        VSESS< VNALGWXJ::VRMeshHoldPatching          >  esHolePatch  ;
//		VSESS< VNALGEQ::VRSprsResolverD				 >  esResolver   ;
//		VSESS< VNALGEQ::VRTriple2MatrixD			 >  esTripleMatr ;
//    } ;
//  
//    VD_PIPE_NS( VNTrnCutTrans_5 , VRProductorFactory
//                                , VSProductorCreatorEnv 
//                                , VSESS< VNTrnPrdct::VRProductor >  ) ;
//    
//    struct VSChangeableJawEnv 
//    {
//        VSESS< VNALGMESH::VRSliceMeshBoundary        >  esSliceHole  ;
//        VSESS< VNALGMESH::VRPerfGraph                >  esPerfGraph  ;
//        VSESS< VNALGMESH::VRGeodicMesh               >  esGeodic     ;  
//        VSESS< VNALGMESH::VRDijkstra                 >  esDijkstra   ;
//        VSESS< VNALGMESH::VRFindHole2                >  esFindHole   ;
//        VSESS< VNALGMESH::VRVtMap2SphereCoord        >  esL2CMapper  ;
//        VSESS< VNALGMESH::VRSphereCoordMap2Vt        >  esC2Lmapper  ;
//        VSESS< VNALGMESH::VRMLSInterpCircle          >  esInterpCrcl ;
//        VSESS< VNALGMESH::VRMLSInterp                >  esInterp     ;
//		VSESS< VNALGEQ::VRSprsResolverD				 >  esResolver;			// added by wxj 20190611
//		VSESS< VNALGEQ::VRTriple2MatrixD			 >  esTripleMatr;		// added by wxj 20190611
//    } ;
//
//    VD_PIPE_NS( VNTrnCutTrans_5 , VRChangeableJawFactory , VSChangeableJawEnv , VSESS< VNAlgShapeJaw::VRChangeableJaw > ) ;
//} ;
//
//namespace VNTrnCutTrans_4to5
//{
//    //VD_ENTRY_NS( VNTrnCutTrans_4to5 , VRJawCreatorENV , VSESS< VNCollisionTest::VRCollisionTest >
//    //                                                  , VSESS< VNALGSEGMENT::VRJawSegment       > ) ;
//
//    //VD_PIPE_NS( VNTrnCutTrans_4to5 , VRTranserCreatorFactory 
//    //                               , VSESS< VRJawCreatorENV > 
//    //                               , VSESS< VRTranserCreator >  ) ;
//
//    struct VSTranserCreatorEnv
//    {
//        VSESS< VNALGMESH::VRPerfGraph           >  esPerfGraph    ;
//        VSESS< VNALGMESH::VRGeodicMesh          >  esGeodic       ;
//        VSESS< VNALGMESH::VRSliceMeshBoundary   >  esBndry        ;
//        //VSESS< VNALGMESH::VRLapComb             >  esLapComb      ;
//        //VSESS< VNALGMESH::VRCurveToMesh         >  esCurv2Mesh    ;
//        //VSESS< VNALGMESH::VRDijkstra            >  esDijkstra     ;
//        //VSESS< VNALGMESH::VRMLSInterpCircle     >  esInterpCircle ;
//        VSESS< VNALGDSGN::VRPatchTooth          >  esPatchTooth   ;
//        VSESS< VNCollisionTest::VRCollisionTest >  esCollision    ;
//        VSESS< VNALGSEGMENT::VRJawSegment       >  esSegment      ; 
//    } ;
//
//    VD_PIPE_NS( VNTrnCutTrans_4to5 , VRTranserCreatorFactory2 
//                                   , VSTranserCreatorEnv 
//                                   , VSESS< VRTranserCreator >  ) ;
//} ;

//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNTrnCutTrans_5::VRChangeableJawFactory      ) ; 
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNTrnCutTrans_5::VRProductorFactory          ) ; 
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNTrnCutTrans_4to5::VRTranserCreatorFactory  ) ;
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNTrnCutTrans_4to5::VRTranserCreatorFactory2 ) ;
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNSysDental::VRAlgSegmentation2              ) ;
LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGMESH::VRPerfMesh                        ) ; 
LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGMESH::VRPerfGraph                       ) ; 
LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGMESH::VRSliceMeshBoundary               ) ; 
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGMESH::VRRefineTooth2                    ) ; 
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGDSGN::VRAlgPatching                     ) ; 


//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNTrnCutTrans_5::VRCutJawCreator             ) ;  
//LDCK4CCE_API VD_IMPORT_DLL_SYSTEM( ldck4cce , VNALGEQ::VRUniformLaplaceMatOfMeshF          ) ; 

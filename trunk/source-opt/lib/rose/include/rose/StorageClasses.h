/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStaticDataManagingClassStorageClassHeader.macro" */
/* JH (01/01/2006) This file is generated using ROSETTA. It should never be 
   manipulated by hand. The generation is located in buildStorageClasses.C!

    This file contains all declarations for the StorageClasses used for the ast file IO.
*/

#include "StorageClassMemoryManagement.h"

class SgSymbolTableStorageClass;
class AstSpecificDataManagingClass;

class AstSpecificDataManagingClassStorageClass
   {

    protected:
     unsigned long rootOfAst;
     unsigned long listOfAccumulatedPoolSizes [ 965 + 1 ];


     unsigned long storageOf_SgNode_globalFunctionTypeTable;
     unsigned long storageOf_SgNode_globalTypeTable;
     EasyStorage < std::map<SgNode*,std::string> > storageOf_SgNode_globalMangledNameMap;
     EasyStorage < std::map<std::string, int> > storageOf_SgNode_shortMangledNameCache;
     EasyStorage < std::map<SgNode*,std::string> > storageOf_SgNode_globalQualifiedNameMapForNames;
     EasyStorage < std::map<SgNode*,std::string> > storageOf_SgNode_globalQualifiedNameMapForTypes;
     EasyStorage < std::map<SgNode*,std::string> > storageOf_SgNode_globalQualifiedNameMapForTemplateHeaders;
     EasyStorage < std::map<SgNode*,std::string> > storageOf_SgNode_globalTypeNameMap;
     EasyStorage < std::map<SgNode*,std::map<SgNode*,std::string> > > storageOf_SgNode_globalQualifiedNameMapForMapsOfTypes;
     EasyStorage < SgNodeSet > storageOf_SgSymbolTable_aliasSymbolCausalNodeSet;
     bool storageOf_SgSymbolTable_force_search_of_base_classes;
     EasyStorage < std::map<int, std::string> > storageOf_Sg_File_Info_fileidtoname_map;
     EasyStorage < std::map<std::string, int> > storageOf_Sg_File_Info_nametofileid_map;
     bool storageOf_SgFile_skip_unparse_asm_commands;
     EasyStorage < SgTypePtrList > storageOf_SgUnparse_Info_structureTagProcessingList;
     bool storageOf_SgUnparse_Info_forceDefaultConstructorToTriggerError;
     bool storageOf_SgUnparse_Info_extern_C_with_braces;
     int storageOf_SgGraph_index_counter;
     int storageOf_SgGraphNode_index_counter;
     int storageOf_SgGraphEdge_index_counter;
     unsigned long storageOf_SgTypeUnknown_builtin_type;
     unsigned long storageOf_SgTypeChar_builtin_type;
     unsigned long storageOf_SgTypeSignedChar_builtin_type;
     unsigned long storageOf_SgTypeUnsignedChar_builtin_type;
     unsigned long storageOf_SgTypeShort_builtin_type;
     unsigned long storageOf_SgTypeSignedShort_builtin_type;
     unsigned long storageOf_SgTypeUnsignedShort_builtin_type;
     unsigned long storageOf_SgTypeInt_builtin_type;
     unsigned long storageOf_SgTypeSignedInt_builtin_type;
     unsigned long storageOf_SgTypeUnsignedInt_builtin_type;
     unsigned long storageOf_SgTypeLong_builtin_type;
     unsigned long storageOf_SgTypeSignedLong_builtin_type;
     unsigned long storageOf_SgTypeUnsignedLong_builtin_type;
     unsigned long storageOf_SgTypeVoid_builtin_type;
     unsigned long storageOf_SgTypeGlobalVoid_builtin_type;
     unsigned long storageOf_SgTypeWchar_builtin_type;
     unsigned long storageOf_SgTypeFloat_builtin_type;
     unsigned long storageOf_SgTypeDouble_builtin_type;
     unsigned long storageOf_SgTypeLongLong_builtin_type;
     unsigned long storageOf_SgTypeSignedLongLong_builtin_type;
     unsigned long storageOf_SgTypeUnsignedLongLong_builtin_type;
     unsigned long storageOf_SgTypeSigned128bitInteger_builtin_type;
     unsigned long storageOf_SgTypeUnsigned128bitInteger_builtin_type;
     unsigned long storageOf_SgTypeFloat80_builtin_type;
     unsigned long storageOf_SgTypeLongDouble_builtin_type;
     unsigned long storageOf_SgTypeBool_builtin_type;
     unsigned long storageOf_SgNamedType_builtin_type;
     unsigned long storageOf_SgPartialFunctionModifierType_builtin_type;
     unsigned long storageOf_SgTypeEllipse_builtin_type;
     unsigned long storageOf_SgTypeDefault_builtin_type;
     unsigned long storageOf_SgTypeCAFTeam_builtin_type;
     unsigned long storageOf_SgTypeCrayPointer_builtin_type;
     unsigned long storageOf_SgTypeLabel_builtin_type;
     unsigned long storageOf_SgTypeNullptr_builtin_type;
     unsigned long storageOf_SgTypeMatrix_builtin_type;
     unsigned long storageOf_SgTypeTuple_builtin_type;
     unsigned long storageOf_SgTypeChar16_builtin_type;
     unsigned long storageOf_SgTypeChar32_builtin_type;
     unsigned long storageOf_SgTypeFloat128_builtin_type;

/* #line 21 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStaticDataManagingClassStorageClassHeader.macro" */

    public:
      void pickOutIRNodeData ( AstSpecificDataManagingClass* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      friend class AstSpecificDataManagingClass;
   };

#if 0 // done in AST_FILE_IO.h
typedef AstSpecificDataManagingClass AstData;
#endif
typedef AstSpecificDataManagingClassStorageClass AstDataStorageClass;

/* #line 98 "../../../src/frontend/SageIII//StorageClasses.h" */


/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNodeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNodeStorageClass  
   {

    protected: 


/* #line 112 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_parent;
      bool storageOf_isModified;
      bool storageOf_containsTransformation;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNode* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNode;
   };
/* #line 134 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSupportStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSupportStorageClass  : public SgNodeStorageClass
   {

    protected: 


/* #line 149 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSupport* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSupport;
   };
/* #line 168 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModifierStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 183 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModifier;
   };
/* #line 202 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModifierNodesStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModifierNodesStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 217 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgModifierTypePtrVector > storageOf_nodes;
     unsigned long storageOf_next;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModifierNodes* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModifierNodes;
   };
/* #line 238 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgConstVolatileModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgConstVolatileModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 253 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgConstVolatileModifier::cv_modifier_enum storageOf_modifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgConstVolatileModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgConstVolatileModifier;
   };
/* #line 273 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStorageModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStorageModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 288 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgStorageModifier::storage_modifier_enum storageOf_modifier;
      bool storageOf_thread_local_storage;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStorageModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStorageModifier;
   };
/* #line 309 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAccessModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAccessModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 324 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgAccessModifier::access_modifier_enum storageOf_modifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAccessModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAccessModifier;
   };
/* #line 344 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 359 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgBitVector > storageOf_modifierVector;
      unsigned long int storageOf_gnu_attribute_constructor_destructor_priority;
       EasyStorage < std::string > storageOf_gnu_attribute_named_weak_reference;
       EasyStorage < std::string > storageOf_gnu_attribute_named_alias;
     unsigned long storageOf_opencl_vec_type;
      SgFunctionModifier::opencl_work_group_size_t storageOf_opencl_work_group_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionModifier;
   };
/* #line 384 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUPC_AccessModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUPC_AccessModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 399 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgUPC_AccessModifier::upc_access_modifier_enum storageOf_modifier;
      bool storageOf_isShared;
      long storageOf_layout;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUPC_AccessModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUPC_AccessModifier;
   };
/* #line 421 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSpecialFunctionModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSpecialFunctionModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 436 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgBitVector > storageOf_modifierVector;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSpecialFunctionModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSpecialFunctionModifier;
   };
/* #line 456 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElaboratedTypeModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElaboratedTypeModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 471 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgElaboratedTypeModifier::elaborated_type_modifier_enum storageOf_modifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElaboratedTypeModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElaboratedTypeModifier;
   };
/* #line 491 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLinkageModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLinkageModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 506 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgLinkageModifier::linkage_modifier_enum storageOf_modifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLinkageModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLinkageModifier;
   };
/* #line 526 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBaseClassModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBaseClassModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 541 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgBaseClassModifier::baseclass_modifier_enum storageOf_modifier;
      SgAccessModifierStorageClass storageOf_accessModifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBaseClassModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBaseClassModifier;
   };
/* #line 562 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStructureModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStructureModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 577 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgStructureModifier::jovial_structure_modifier_enum storageOf_modifier;
      int storageOf_bits_per_entry;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStructureModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStructureModifier;
   };
/* #line 598 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 613 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgBitVector > storageOf_modifierVector;
      SgUPC_AccessModifierStorageClass storageOf_upcModifier;
      SgStructureModifierStorageClass storageOf_structureModifier;
      SgConstVolatileModifierStorageClass storageOf_constVolatileModifier;
      SgElaboratedTypeModifierStorageClass storageOf_elaboratedTypeModifier;
      SgTypeModifier::gnu_extension_machine_mode_enum storageOf_gnu_extension_machine_mode;
      int storageOf_gnu_attribute_alignment;
      long storageOf_gnu_attribute_sentinel;
      unsigned storageOf_address_space_value;
      unsigned storageOf_vector_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeModifier;
   };
/* #line 642 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeclarationModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeclarationModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 657 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgBitVector > storageOf_modifierVector;
      SgTypeModifierStorageClass storageOf_typeModifier;
      SgAccessModifierStorageClass storageOf_accessModifier;
      SgStorageModifierStorageClass storageOf_storageModifier;
       EasyStorage < std::string > storageOf_gnu_attribute_section_name;
      SgDeclarationModifier::gnu_declaration_visability_enum storageOf_gnu_attribute_visability;
       EasyStorage < std::string > storageOf_microsoft_uuid_string;
       EasyStorage < std::string > storageOf_microsoft_property_get_function_name;
       EasyStorage < std::string > storageOf_microsoft_property_put_function_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeclarationModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeclarationModifier;
   };
/* #line 685 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOpenclAccessModeModifierStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOpenclAccessModeModifierStorageClass  : public SgModifierStorageClass
   {

    protected: 


/* #line 700 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOpenclAccessModeModifier::access_mode_modifier_enum storageOf_modifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOpenclAccessModeModifier* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOpenclAccessModeModifier;
   };
/* #line 720 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNameStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNameStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 735 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_char;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgName* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgName;
   };
/* #line 755 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSymbolTableStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSymbolTableStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 770 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      bool storageOf_no_name;
       EasyStorage <  rose_hash_multimap* > storageOf_table;
       EasyStorage < SgNodeSet > storageOf_symbolSet;
      bool storageOf_case_insensitive;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSymbolTable* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSymbolTable;
   };
/* #line 794 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAttributeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAttributeStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 809 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAttribute* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAttribute;
   };
/* #line 829 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPragmaStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPragmaStorageClass  : public SgAttributeStorageClass
   {

    protected: 


/* #line 844 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_startOfConstruct;
     unsigned long storageOf_endOfConstruct;
      short storageOf_printed;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPragma* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPragma;
   };
/* #line 867 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitAttributeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitAttributeStorageClass  : public SgAttributeStorageClass
   {

    protected: 


/* #line 882 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned long int storageOf_bitflag;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitAttribute* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitAttribute;
   };
/* #line 902 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFuncDecl_attrStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFuncDecl_attrStorageClass  : public SgBitAttributeStorageClass
   {

    protected: 


/* #line 917 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFuncDecl_attr* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFuncDecl_attr;
   };
/* #line 936 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassDecl_attrStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassDecl_attrStorageClass  : public SgBitAttributeStorageClass
   {

    protected: 


/* #line 951 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassDecl_attr* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassDecl_attr;
   };
/* #line 970 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for Sg_File_InfoStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class Sg_File_InfoStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 985 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_file_id;
      int storageOf_line;
      int storageOf_col;
      unsigned int storageOf_classificationBitField;
      int storageOf_physical_file_id;
      int storageOf_physical_line;
      unsigned int storageOf_source_sequence_number;
       EasyStorage < SgFileIdList > storageOf_fileIDsToUnparse;
       EasyStorage < SgFileLineNumberList > storageOf_fileLineNumbersToUnparse;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( Sg_File_Info* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class Sg_File_Info;
   };
/* #line 1013 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFileStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFileStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1028 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_startOfConstruct;
       EasyStorage < SgStringList > storageOf_originalCommandLineArgumentList;
      int storageOf_verbose;
      bool storageOf_output_warnings;
      bool storageOf_C_only;
      bool storageOf_Cxx_only;
      bool storageOf_Fortran_only;
      bool storageOf_CoArrayFortran_only;
      int storageOf_upc_threads;
      bool storageOf_Java_only;
      bool storageOf_Jovial_only;
      bool storageOf_X10_only;
      bool storageOf_PHP_only;
      bool storageOf_Python_only;
      bool storageOf_Cuda_only;
      bool storageOf_OpenCL_only;
      bool storageOf_Csharp_only;
      bool storageOf_Ada_only;
      bool storageOf_Cobol_only;
      bool storageOf_requires_C_preprocessor;
      bool storageOf_binary_only;
      SgFile::outputFormatOption_enum storageOf_inputFormat;
      SgFile::outputFormatOption_enum storageOf_outputFormat;
      SgFile::outputFormatOption_enum storageOf_backendCompileFormat;
      bool storageOf_fortran_implicit_none;
      bool storageOf_openmp;
      bool storageOf_openmp_parse_only;
      bool storageOf_openmp_ast_only;
      bool storageOf_openmp_lowering;
      bool storageOf_openacc;
      bool storageOf_openacc_parse_only;
      bool storageOf_openacc_ast_only;
      bool storageOf_cray_pointer_support;
      bool storageOf_failsafe;
      bool storageOf_output_parser_actions;
      bool storageOf_exit_after_parser;
      bool storageOf_skip_syntax_check;
      bool storageOf_skip_parser;
      bool storageOf_relax_syntax_check;
      bool storageOf_skip_translation_from_edg_ast_to_rose_ast;
      bool storageOf_skip_transformation;
      bool storageOf_skip_unparse;
      bool storageOf_skipfinalCompileStep;
      bool storageOf_unparse_includes;
      bool storageOf_unparse_line_directives;
      bool storageOf_unparse_function_calls_using_operator_syntax;
      bool storageOf_unparse_function_calls_using_operator_names;
      bool storageOf_unparse_instruction_addresses;
      bool storageOf_unparse_raw_memory_contents;
      bool storageOf_unparse_binary_file_format;
      SgFile::languageOption_enum storageOf_outputLanguage;
      SgFile::languageOption_enum storageOf_inputLanguage;
       EasyStorage < std::string > storageOf_sourceFileNameWithPath;
       EasyStorage < std::string > storageOf_sourceFileNameWithoutPath;
       EasyStorage < std::string > storageOf_unparse_output_filename;
       EasyStorage < std::string > storageOf_objectFileNameWithPath;
       EasyStorage < std::string > storageOf_objectFileNameWithoutPath;
      bool storageOf_useBackendOnly;
      bool storageOf_compileOnly;
       EasyStorage < std::string > storageOf_savedFrontendCommandLine;
      bool storageOf_no_implicit_templates;
      bool storageOf_no_implicit_inline_templates;
      bool storageOf_skip_commentsAndDirectives;
      bool storageOf_collectAllCommentsAndDirectives;
      bool storageOf_translateCommentsAndDirectivesIntoAST;
      bool storageOf_unparseHeaderFiles;
       EasyStorage < ROSEAttributesListContainerPtr > storageOf_preprocessorDirectivesAndCommentsList;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
      bool storageOf_KCC_frontend;
      bool storageOf_new_frontend;
      bool storageOf_disable_edg_backend;
      bool storageOf_disable_sage_backend;
      int storageOf_testingLevel;
      bool storageOf_preinit_il;
      bool storageOf_enable_cp_backend;
      bool storageOf_markGeneratedFiles;
      bool storageOf_negative_test;
      bool storageOf_strict_language_handling;
      bool storageOf_wave;
      int storageOf_embedColorCodesInGeneratedCode;
      int storageOf_generateSourcePositionCodes;
      bool storageOf_sourceFileUsesCppFileExtension;
      bool storageOf_sourceFileUsesFortranFileExtension;
      bool storageOf_sourceFileUsesFortran77FileExtension;
      bool storageOf_sourceFileUsesFortran90FileExtension;
      bool storageOf_sourceFileUsesFortran95FileExtension;
      bool storageOf_sourceFileUsesFortran2003FileExtension;
      bool storageOf_sourceFileUsesFortran2008FileExtension;
      bool storageOf_sourceFileUsesCoArrayFortranFileExtension;
      bool storageOf_sourceFileUsesPHPFileExtension;
      bool storageOf_sourceFileUsesPythonFileExtension;
      bool storageOf_sourceFileUsesJavaFileExtension;
      bool storageOf_sourceFileUsesBinaryFileExtension;
      bool storageOf_sourceFileTypeIsUnknown;
      bool storageOf_sourceFileUsesX10FileExtension;
      bool storageOf_sourceFileUsesCsharpFileExtension;
      bool storageOf_sourceFileUsesAdaFileExtension;
      bool storageOf_sourceFileUsesJovialFileExtension;
      bool storageOf_sourceFileUsesCobolFileExtension;
      int storageOf_detect_dangling_pointers;
      bool storageOf_experimental_fortran_frontend;
      bool storageOf_experimental_flang_frontend;
      bool storageOf_experimental_cuda_fortran_frontend;
      bool storageOf_experimental_fortran_frontend_OFP_test;
      bool storageOf_read_executable_file_format_only;
      bool storageOf_visualize_executable_file_format_skip_symbols;
      bool storageOf_visualize_dwarf_only;
      bool storageOf_read_instructions_only;
       EasyStorage < SgStringList > storageOf_libraryArchiveObjectFileNameList;
      bool storageOf_isLibraryArchive;
      bool storageOf_isObjectFile;
      bool storageOf_unparse_tokens;
      int storageOf_unparse_tokens_testing;
      bool storageOf_unparse_using_leading_and_trailing_token_mappings;
      bool storageOf_unparse_template_ast;
      bool storageOf_skipAstConsistancyTests;
      bool storageOf_multifile_support;
      bool storageOf_optimization;
      bool storageOf_use_token_stream_to_improve_source_position_info;
      bool storageOf_suppress_variable_declaration_normalization;
      bool storageOf_edg_il_to_graphviz;
      bool storageOf_no_optimize_flag_for_frontend;
      bool storageOf_unparse_edg_normalized_method_ROSE_1392;
      bool storageOf_header_file_unparsing_optimization;
      bool storageOf_header_file_unparsing_optimization_source_file;
      bool storageOf_header_file_unparsing_optimization_header_file;
      SgFile::standard_enum storageOf_standard;
      bool storageOf_gnu_standard;
      int storageOf_frontendErrorCode;
      int storageOf_javacErrorCode;
      int storageOf_ecjErrorCode;
      int storageOf_midendErrorCode;
      int storageOf_unparserErrorCode;
      int storageOf_backendCompilerErrorCode;
      bool storageOf_unparsedFileFailedCompilation;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFile* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFile;
   };
/* #line 1182 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSourceFileStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSourceFileStorageClass  : public SgFileStorageClass
   {

    protected: 


/* #line 1197 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_globalScope;
       EasyStorage < SgModuleStatementPtrList > storageOf_module_list;
       EasyStorage < SgTokenPtrList > storageOf_token_list;
     unsigned long storageOf_temp_holding_scope;
     unsigned long storageOf_package;
     unsigned long storageOf_import_list;
     unsigned long storageOf_class_list;
      bool storageOf_isHeaderFile;
      bool storageOf_isHeaderFileIncludedMoreThanOnce;
     unsigned long storageOf_headerFileReport;
       EasyStorage < SgStringList > storageOf_extraIncludeDirectorySpecifierBeforeList;
       EasyStorage < SgStringList > storageOf_extraIncludeDirectorySpecifierAfterList;
     unsigned long storageOf_associated_include_file;
      bool storageOf_processedToIncludeCppDirectivesAndComments;
       EasyStorage < SgNodePtrList > storageOf_extra_nodes_for_namequal_init;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSourceFile* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSourceFile;
   };
/* #line 1231 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnknownFileStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnknownFileStorageClass  : public SgFileStorageClass
   {

    protected: 


/* #line 1246 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_globalScope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnknownFile* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnknownFile;
   };
/* #line 1266 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgProjectStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgProjectStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1281 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_fileList_ptr;
       EasyStorage < SgStringList > storageOf_originalCommandLineArgumentList;
      int storageOf_frontendErrorCode;
      int storageOf_javacErrorCode;
      int storageOf_ecjErrorCode;
      int storageOf_midendErrorCode;
      int storageOf_backendErrorCode;
      bool storageOf_keep_going;
      bool storageOf_unparser__clobber_input_file;
       EasyStorage < std::string > storageOf_outputFileName;
       EasyStorage < SgStringList > storageOf_sourceFileNameList;
       EasyStorage < SgStringList > storageOf_objectFileNameList;
       EasyStorage < SgStringList > storageOf_libraryFileList;
       EasyStorage < SgStringList > storageOf_librarySpecifierList;
       EasyStorage < SgStringList > storageOf_libraryDirectorySpecifierList;
       EasyStorage < SgStringList > storageOf_includeDirectorySpecifierList;
       EasyStorage < SgStringList > storageOf_macroSpecifierList;
       EasyStorage < SgStringList > storageOf_preincludeFileList;
       EasyStorage < SgStringList > storageOf_preincludeDirectoryList;
      bool storageOf_compileOnly;
      bool storageOf_wave;
      bool storageOf_prelink;
      SgProject::template_instantiation_enum storageOf_template_instantiation_mode;
      bool storageOf_ast_merge;
       EasyStorage < std::string > storageOf_projectSpecificDatabaseFile;
      bool storageOf_C_PreprocessorOnly;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
       EasyStorage < std::string > storageOf_compilationPerformanceFile;
       EasyStorage < SgStringList > storageOf_includePathList;
       EasyStorage < SgStringList > storageOf_excludePathList;
       EasyStorage < SgStringList > storageOf_includeFileList;
       EasyStorage < SgStringList > storageOf_excludeFileList;
      bool storageOf_binary_only;
       EasyStorage < std::string > storageOf_dataBaseFilename;
     unsigned long storageOf_directoryList;
      bool storageOf_C_only;
      bool storageOf_Cxx_only;
      bool storageOf_C11_only;
      bool storageOf_Cxx0x_only;
      bool storageOf_Cxx11_only;
      bool storageOf_C14_only;
      bool storageOf_Cxx14_only;
      bool storageOf_Fortran_only;
       EasyStorage < std::list<std::string> > storageOf_Fortran_ofp_jvm_options;
      bool storageOf_Java_only;
      bool storageOf_Jovial_only;
      bool storageOf_X10_only;
      bool storageOf_Ada_only;
      bool storageOf_openmp_linking;
       EasyStorage < std::list<std::string> > storageOf_Java_ecj_jvm_options;
      bool storageOf_Java_batch_mode;
       EasyStorage < std::list<std::string> > storageOf_Java_classpath;
       EasyStorage < std::list<std::string> > storageOf_Java_sourcepath;
       EasyStorage < std::string > storageOf_Java_destdir;
       EasyStorage < std::string > storageOf_Java_source_destdir;
       EasyStorage < std::string > storageOf_Java_s;
       EasyStorage < std::string > storageOf_Java_source;
       EasyStorage < std::string > storageOf_Java_target;
       EasyStorage < std::string > storageOf_Java_encoding;
       EasyStorage < std::string > storageOf_Java_g;
      bool storageOf_Java_nowarn;
      bool storageOf_Java_verbose;
      bool storageOf_Java_deprecation;
       EasyStorage < std::list<std::string> > storageOf_Java_bootclasspath;
      bool storageOf_addCppDirectivesToAST;
       EasyStorage < std::map<std::string, std::set<PreprocessingInfo*> > > storageOf_includingPreprocessingInfosMap;
       EasyStorage < std::list<std::string> > storageOf_quotedIncludesSearchPaths;
       EasyStorage < std::list<std::string> > storageOf_bracketedIncludesSearchPaths;
       EasyStorage < std::string > storageOf_unparseHeaderFilesRootFolder;
      bool storageOf_frontendConstantFolding;
     unsigned long storageOf_globalScopeAcrossFiles;
      bool storageOf_unparse_in_same_directory_as_input_file;
      bool storageOf_stop_after_compilation_do_not_assemble_file;
       EasyStorage < std::string > storageOf_gnuOptionForUndefinedSymbol;
      bool storageOf_mode_32_bit;
      bool storageOf_noclobber_output_file;
      bool storageOf_noclobber_if_different_output_file;
      bool storageOf_suppressConstantFoldingPostProcessing;
      bool storageOf_appendPID;
      bool storageOf_reportOnHeaderFileUnparsing;
       EasyStorage < std::string > storageOf_applicationRootDirectory;
      bool storageOf_usingApplicationRootDirectory;
      bool storageOf_usingDeferredTransformations;
       EasyStorage < std::string > storageOf_astfile_out;
       EasyStorage < std::list<std::string> > storageOf_astfiles_in;
       EasyStorage < SgStringList > storageOf_extraIncludeDirectorySpecifierBeforeList;
       EasyStorage < SgStringList > storageOf_extraIncludeDirectorySpecifierAfterList;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgProject* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgProject;
   };
/* #line 1387 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOptionsStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOptionsStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1402 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_debug_level;
      int storageOf_logging_level;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOptions* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOptions;
   };
/* #line 1423 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnparse_InfoStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnparse_InfoStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1438 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgBitVector > storageOf_unparse_attribute;
      int storageOf_access_attribute;
      int storageOf_nested_expression;
       EasyStorage < std::string > storageOf_operator_name;
      SgNameStorageClass storageOf_var_name;
     unsigned long storageOf_declstatement_ptr;
     unsigned long storageOf_current_context;
      SgNameStorageClass storageOf_array_index_list;
     unsigned long storageOf_current_namespace;
      bool storageOf_outputCodeGenerationFormatDelimiters;
       EasyStorage < SgQualifiedNamePtrList > storageOf_qualifiedNameList;
     unsigned long storageOf_current_function_call;
     unsigned long storageOf_current_scope;
     unsigned long storageOf_reference_node_for_qualification;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      int storageOf_nestingLevel;
      SgFile::languageOption_enum storageOf_language;
     unsigned long storageOf_current_source_file;
      bool storageOf_use_generated_name_for_template_arguments;
      bool storageOf_user_defined_literal;
     unsigned long storageOf_declstatement_associated_with_type;
      bool storageOf_context_for_added_parentheses;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnparse_Info* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnparse_Info;
   };
/* #line 1481 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBaseClassStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBaseClassStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1496 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_class;
      bool storageOf_isDirectBaseClass;
     unsigned long storageOf_baseClassModifier;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBaseClass* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBaseClass;
   };
/* #line 1521 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExpBaseClassStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExpBaseClassStorageClass  : public SgBaseClassStorageClass
   {

    protected: 


/* #line 1536 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_class_exp;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExpBaseClass* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExpBaseClass;
   };
/* #line 1556 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonrealBaseClassStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonrealBaseClassStorageClass  : public SgBaseClassStorageClass
   {

    protected: 


/* #line 1571 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_class_nonreal;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonrealBaseClass* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonrealBaseClass;
   };
/* #line 1591 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypedefSeqStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypedefSeqStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1606 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTypePtrList > storageOf_typedefs;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypedefSeq* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypedefSeq;
   };
/* #line 1626 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateParameterStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateParameterStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1641 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgTemplateParameter::template_parameter_enum storageOf_parameterType;
     unsigned long storageOf_type;
     unsigned long storageOf_defaultTypeParameter;
     unsigned long storageOf_expression;
     unsigned long storageOf_defaultExpressionParameter;
     unsigned long storageOf_templateDeclaration;
     unsigned long storageOf_defaultTemplateDeclarationParameter;
     unsigned long storageOf_initializedName;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateParameter* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateParameter;
   };
/* #line 1668 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateArgumentStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateArgumentStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1683 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgTemplateArgument::template_argument_enum storageOf_argumentType;
      bool storageOf_isArrayBoundUnknownType;
     unsigned long storageOf_type;
     unsigned long storageOf_unparsable_type_alias;
     unsigned long storageOf_expression;
     unsigned long storageOf_templateDeclaration;
     unsigned long storageOf_initializedName;
      bool storageOf_explicitlySpecified;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_requiresGlobalNameQualificationOnType;
      int storageOf_name_qualification_length_for_type;
      bool storageOf_type_elaboration_required_for_type;
      bool storageOf_global_qualification_required_for_type;
     unsigned long storageOf_previous_instance;
     unsigned long storageOf_next_instance;
      bool storageOf_is_pack_element;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateArgument* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateArgument;
   };
/* #line 1720 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDirectoryStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDirectoryStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1735 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_fileList;
     unsigned long storageOf_directoryList;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDirectory* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDirectory;
   };
/* #line 1758 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFileListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFileListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1773 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgFilePtrList > storageOf_listOfFiles;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFileList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFileList;
   };
/* #line 1794 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDirectoryListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDirectoryListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1809 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDirectoryPtrList > storageOf_listOfDirectories;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDirectoryList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDirectoryList;
   };
/* #line 1829 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionParameterTypeListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionParameterTypeListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1844 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTypePtrList > storageOf_arguments;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionParameterTypeList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionParameterTypeList;
   };
/* #line 1864 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgQualifiedNameStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgQualifiedNameStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1879 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgQualifiedName* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgQualifiedName;
   };
/* #line 1899 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateArgumentListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateArgumentListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1914 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateArgumentPtrList > storageOf_args;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateArgumentList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateArgumentList;
   };
/* #line 1934 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateParameterListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateParameterListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1949 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_args;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateParameterList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateParameterList;
   };
/* #line 1969 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTypeConstraintStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTypeConstraintStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 1984 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTypeConstraint* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTypeConstraint;
   };
/* #line 2003 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaRangeConstraintStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaRangeConstraintStorageClass  : public SgAdaTypeConstraintStorageClass
   {

    protected: 


/* #line 2018 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_range;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaRangeConstraint* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaRangeConstraint;
   };
/* #line 2038 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGraphStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2053 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
       EasyStorage < rose_graph_integer_node_hash_map > storageOf_node_index_to_node_map;
       EasyStorage < rose_graph_integer_edge_hash_map > storageOf_edge_index_to_edge_map;
       EasyStorage < rose_graph_integerpair_edge_hash_multimap > storageOf_node_index_pair_to_edge_multimap;
       EasyStorage < rose_graph_string_integer_hash_multimap > storageOf_string_to_node_index_multimap;
       EasyStorage < rose_graph_string_integer_hash_multimap > storageOf_string_to_edge_index_multimap;
       EasyStorage < rose_graph_integer_edge_hash_multimap > storageOf_node_index_to_edge_multimap;
      int storageOf_index;
       EasyStorage < SgBoostEdgeList > storageOf_boost_edges;
       EasyStorage < SgBoostEdgeWeightList > storageOf_boost_edge_weights;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
       EasyStorage < std::map<int, std::string> > storageOf_properties;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGraph;
   };
/* #line 2084 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIncidenceDirectedGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIncidenceDirectedGraphStorageClass  : public SgGraphStorageClass
   {

    protected: 


/* #line 2099 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < rose_graph_integer_edge_hash_multimap > storageOf_node_index_to_edge_multimap_edgesOut;
       EasyStorage < rose_graph_integer_edge_hash_multimap > storageOf_node_index_to_edge_multimap_edgesIn;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIncidenceDirectedGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIncidenceDirectedGraph;
   };
/* #line 2120 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBidirectionalGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBidirectionalGraphStorageClass  : public SgIncidenceDirectedGraphStorageClass
   {

    protected: 


/* #line 2135 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBidirectionalGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBidirectionalGraph;
   };
/* #line 2154 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStringKeyedBidirectionalGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStringKeyedBidirectionalGraphStorageClass  : public SgBidirectionalGraphStorageClass
   {

    protected: 


/* #line 2169 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStringKeyedBidirectionalGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStringKeyedBidirectionalGraph;
   };
/* #line 2188 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIntKeyedBidirectionalGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIntKeyedBidirectionalGraphStorageClass  : public SgBidirectionalGraphStorageClass
   {

    protected: 


/* #line 2203 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIntKeyedBidirectionalGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIntKeyedBidirectionalGraph;
   };
/* #line 2222 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIncidenceUndirectedGraphStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIncidenceUndirectedGraphStorageClass  : public SgGraphStorageClass
   {

    protected: 


/* #line 2237 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIncidenceUndirectedGraph* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIncidenceUndirectedGraph;
   };
/* #line 2256 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGraphNodeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGraphNodeStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2271 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_SgNode;
      int storageOf_index;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
       EasyStorage < std::map<int, std::string> > storageOf_properties;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGraphNode* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGraphNode;
   };
/* #line 2295 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGraphEdgeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGraphEdgeStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2310 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_node_A;
     unsigned long storageOf_node_B;
       EasyStorage < std::string > storageOf_name;
      int storageOf_index;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
       EasyStorage < std::map<int, std::string> > storageOf_properties;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGraphEdge* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGraphEdge;
   };
/* #line 2335 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDirectedGraphEdgeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDirectedGraphEdgeStorageClass  : public SgGraphEdgeStorageClass
   {

    protected: 


/* #line 2350 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDirectedGraphEdge* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDirectedGraphEdge;
   };
/* #line 2369 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUndirectedGraphEdgeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUndirectedGraphEdgeStorageClass  : public SgGraphEdgeStorageClass
   {

    protected: 


/* #line 2384 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUndirectedGraphEdge* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUndirectedGraphEdge;
   };
/* #line 2403 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGraphNodeListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGraphNodeListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2418 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGraphNodeList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGraphNodeList;
   };
/* #line 2437 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGraphEdgeListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGraphEdgeListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2452 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGraphEdgeList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGraphEdgeList;
   };
/* #line 2471 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeTableStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeTableStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2486 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type_table;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeTable* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeTable;
   };
/* #line 2506 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNameGroupStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNameGroupStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2521 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_group_name;
       EasyStorage < SgStringList > storageOf_name_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNameGroup* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNameGroup;
   };
/* #line 2542 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDimensionObjectStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDimensionObjectStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2557 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_array;
     unsigned long storageOf_shape;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDimensionObject* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDimensionObject;
   };
/* #line 2578 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFormatItemStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFormatItemStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2593 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_repeat_specification;
     unsigned long storageOf_data;
     unsigned long storageOf_format_item_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFormatItem* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFormatItem;
   };
/* #line 2615 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFormatItemListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFormatItemListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2630 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgFormatItemPtrList > storageOf_format_item_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFormatItemList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFormatItemList;
   };
/* #line 2650 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDataStatementGroupStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDataStatementGroupStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2665 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDataStatementObjectPtrList > storageOf_object_list;
       EasyStorage < SgDataStatementValuePtrList > storageOf_value_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDataStatementGroup* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDataStatementGroup;
   };
/* #line 2686 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDataStatementObjectStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDataStatementObjectStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2701 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_variableReference_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDataStatementObject* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDataStatementObject;
   };
/* #line 2721 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIncludeFileStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIncludeFileStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2736 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_filename;
     unsigned long storageOf_source_file;
       EasyStorage < SgIncludeFilePtrList > storageOf_include_file_list;
      unsigned int storageOf_first_source_sequence_number;
      unsigned int storageOf_last_source_sequence_number;
      bool storageOf_isIncludedMoreThanOnce;
      bool storageOf_isPrimaryUse;
       EasyStorage < std::string > storageOf_file_hash;
      SgNameStorageClass storageOf_name_used_in_include_directive;
     unsigned long storageOf_source_file_of_translation_unit;
     unsigned long storageOf_including_source_file;
     unsigned long storageOf_parent_include_file;
      bool storageOf_isSystemInclude;
      bool storageOf_isPreinclude;
      bool storageOf_requires_explict_path_for_unparsed_headers;
      bool storageOf_can_be_supported_using_token_based_unparsing;
      SgNameStorageClass storageOf_directory_prefix;
      SgNameStorageClass storageOf_name_without_path;
      SgNameStorageClass storageOf_applicationRootDirectory;
      bool storageOf_will_be_unparsed;
      bool storageOf_isRoseSystemInclude;
      bool storageOf_from_system_include_dir;
      bool storageOf_preinclude_macros_only;
      bool storageOf_isApplicationFile;
      bool storageOf_isRootSourceFile;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIncludeFile* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIncludeFile;
   };
/* #line 2780 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDataStatementValueStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDataStatementValueStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2795 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgDataStatementValue::data_statement_value_enum storageOf_data_initialization_format;
     unsigned long storageOf_initializer_list;
     unsigned long storageOf_repeat_expression;
     unsigned long storageOf_constant_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDataStatementValue* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDataStatementValue;
   };
/* #line 2818 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaImportStatementListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaImportStatementListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2833 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgJavaImportStatementPtrList > storageOf_java_import_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaImportStatementList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaImportStatementList;
   };
/* #line 2853 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaClassDeclarationListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaClassDeclarationListStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2868 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgClassDeclarationPtrList > storageOf_java_class_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaClassDeclarationList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaClassDeclarationList;
   };
/* #line 2888 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgHeaderFileReportStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgHeaderFileReportStorageClass  : public SgSupportStorageClass
   {

    protected: 


/* #line 2903 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_source_file;
       EasyStorage < SgSourceFilePtrList > storageOf_include_file_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgHeaderFileReport* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgHeaderFileReport;
   };
/* #line 2924 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeStorageClass  : public SgNodeStorageClass
   {

    protected: 


/* #line 2939 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_isCoArray;
      int storageOf_substitutedForTemplateParam;
     unsigned long storageOf_ref_to;
     unsigned long storageOf_ptr_to;
     unsigned long storageOf_modifiers;
     unsigned long storageOf_typedefs;
     unsigned long storageOf_rvalue_ref_to;
     unsigned long storageOf_decltype_ref_to;
     unsigned long storageOf_typeof_ref_to;
     unsigned long storageOf_type_kind;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgType;
   };
/* #line 2969 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnknownStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnknownStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 2984 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_type_name;
      bool storageOf_has_type_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnknown* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnknown;
   };
/* #line 3005 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeCharStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeCharStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3020 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeChar* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeChar;
   };
/* #line 3039 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSignedCharStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSignedCharStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3054 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSignedChar* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSignedChar;
   };
/* #line 3073 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsignedCharStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsignedCharStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3088 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsignedChar* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsignedChar;
   };
/* #line 3107 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeShortStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeShortStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3122 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeShort* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeShort;
   };
/* #line 3141 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSignedShortStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSignedShortStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3156 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSignedShort* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSignedShort;
   };
/* #line 3175 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsignedShortStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsignedShortStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3190 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsignedShort* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsignedShort;
   };
/* #line 3209 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeIntStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeIntStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3224 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_field_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeInt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeInt;
   };
/* #line 3244 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSignedIntStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSignedIntStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3259 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSignedInt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSignedInt;
   };
/* #line 3278 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsignedIntStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsignedIntStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3293 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsignedInt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsignedInt;
   };
/* #line 3312 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3327 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeLong;
   };
/* #line 3346 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSignedLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSignedLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3361 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSignedLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSignedLong;
   };
/* #line 3380 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsignedLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsignedLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3395 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsignedLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsignedLong;
   };
/* #line 3414 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeVoidStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeVoidStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3429 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeVoid* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeVoid;
   };
/* #line 3448 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeGlobalVoidStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeGlobalVoidStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3463 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeGlobalVoid* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeGlobalVoid;
   };
/* #line 3482 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeWcharStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeWcharStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3497 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeWchar* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeWchar;
   };
/* #line 3516 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeFloatStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeFloatStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3531 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeFloat* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeFloat;
   };
/* #line 3550 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeDoubleStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeDoubleStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3565 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeDouble* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeDouble;
   };
/* #line 3584 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeLongLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeLongLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3599 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeLongLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeLongLong;
   };
/* #line 3618 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSignedLongLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSignedLongLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3633 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSignedLongLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSignedLongLong;
   };
/* #line 3652 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsignedLongLongStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsignedLongLongStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3667 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsignedLongLong* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsignedLongLong;
   };
/* #line 3686 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeSigned128bitIntegerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeSigned128bitIntegerStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3701 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeSigned128bitInteger* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeSigned128bitInteger;
   };
/* #line 3720 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeUnsigned128bitIntegerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeUnsigned128bitIntegerStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3735 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeUnsigned128bitInteger* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeUnsigned128bitInteger;
   };
/* #line 3754 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeFloat80StorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeFloat80StorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3769 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeFloat80* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeFloat80;
   };
/* #line 3788 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeLongDoubleStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeLongDoubleStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3803 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeLongDouble* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeLongDouble;
   };
/* #line 3822 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeStringStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeStringStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3837 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lengthExpression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeString* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeString;
   };
/* #line 3857 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeBoolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeBoolStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3872 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeBool* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeBool;
   };
/* #line 3891 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPointerTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPointerTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3906 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPointerType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPointerType;
   };
/* #line 3926 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPointerMemberTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPointerMemberTypeStorageClass  : public SgPointerTypeStorageClass
   {

    protected: 


/* #line 3941 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_class_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPointerMemberType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPointerMemberType;
   };
/* #line 3961 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgReferenceTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgReferenceTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 3976 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgReferenceType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgReferenceType;
   };
/* #line 3996 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamedTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamedTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4011 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;
      bool storageOf_autonomous_declaration;
      bool storageOf_is_from_template_parameter;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamedType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamedType;
   };
/* #line 4033 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4048 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_packed;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassType;
   };
/* #line 4068 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaParameterTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaParameterTypeStorageClass  : public SgClassTypeStorageClass
   {

    protected: 


/* #line 4083 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaParameterType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaParameterType;
   };
/* #line 4102 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialTableTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialTableTypeStorageClass  : public SgClassTypeStorageClass
   {

    protected: 


/* #line 4117 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
     unsigned long storageOf_dim_info;
      int storageOf_rank;
      unsigned int storageOf_structure_specifier;
      unsigned int storageOf_bits_per_entry;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialTableType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialTableType;
   };
/* #line 4141 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEnumTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEnumTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4156 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEnumType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEnumType;
   };
/* #line 4175 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypedefTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypedefTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4190 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_parent_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypedefType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypedefType;
   };
/* #line 4210 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonrealTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonrealTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4225 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonrealType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonrealType;
   };
/* #line 4244 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaParameterizedTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaParameterizedTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4259 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_raw_type;
     unsigned long storageOf_type_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaParameterizedType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaParameterizedType;
   };
/* #line 4280 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaQualifiedTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaQualifiedTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4295 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_parent_type;
     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaQualifiedType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaQualifiedType;
   };
/* #line 4316 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaWildcardTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaWildcardTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4331 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_bound_type;
      bool storageOf_is_unbound;
      bool storageOf_has_extends;
      bool storageOf_has_super;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaWildcardType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaWildcardType;
   };
/* #line 4354 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskTypeStorageClass  : public SgNamedTypeStorageClass
   {

    protected: 


/* #line 4369 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_decl;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskType;
   };
/* #line 4389 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModifierTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModifierTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4404 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
      SgTypeModifierStorageClass storageOf_typeModifier;
       EasyStorage < char* > storageOf_frontend_type_reference;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModifierType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModifierType;
   };
/* #line 4426 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4441 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_return_type;
      bool storageOf_has_ellipses;
     unsigned long storageOf_orig_return_type;
     unsigned long storageOf_argument_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionType;
   };
/* #line 4464 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMemberFunctionTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMemberFunctionTypeStorageClass  : public SgFunctionTypeStorageClass
   {

    protected: 


/* #line 4479 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_class_type;
      unsigned int storageOf_mfunc_specifier;
      unsigned int storageOf_ref_qualifiers;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMemberFunctionType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMemberFunctionType;
   };
/* #line 4501 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPartialFunctionTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPartialFunctionTypeStorageClass  : public SgMemberFunctionTypeStorageClass
   {

    protected: 


/* #line 4516 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPartialFunctionType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPartialFunctionType;
   };
/* #line 4535 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPartialFunctionModifierTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPartialFunctionModifierTypeStorageClass  : public SgPartialFunctionTypeStorageClass
   {

    protected: 


/* #line 4550 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPartialFunctionModifierType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPartialFunctionModifierType;
   };
/* #line 4569 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgArrayTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgArrayTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4584 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
     unsigned long storageOf_index;
     unsigned long storageOf_dim_info;
      int storageOf_rank;
      int storageOf_number_of_elements;
      bool storageOf_is_variable_length_array;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgArrayType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgArrayType;
   };
/* #line 4609 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeEllipseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeEllipseStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4624 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeEllipse* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeEllipse;
   };
/* #line 4643 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4658 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      int storageOf_template_parameter_position;
      int storageOf_template_parameter_depth;
     unsigned long storageOf_class_type;
     unsigned long storageOf_parent_class_type;
     unsigned long storageOf_template_parameter;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_tpl_args;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_part_spec_tpl_args;
      bool storageOf_packed;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateType;
   };
/* #line 4686 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgQualifiedNameTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgQualifiedNameTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4701 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
       EasyStorage < SgQualifiedNamePtrList > storageOf_qualifiedNameList;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgQualifiedNameType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgQualifiedNameType;
   };
/* #line 4722 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeComplexStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeComplexStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4737 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeComplex* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeComplex;
   };
/* #line 4757 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeImaginaryStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeImaginaryStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4772 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeImaginary* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeImaginary;
   };
/* #line 4792 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeDefaultStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeDefaultStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4807 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeDefault* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeDefault;
   };
/* #line 4827 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeCAFTeamStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeCAFTeamStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4842 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeCAFTeam* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeCAFTeam;
   };
/* #line 4861 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeCrayPointerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeCrayPointerStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4876 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeCrayPointer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeCrayPointer;
   };
/* #line 4895 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeLabelStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeLabelStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4910 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeLabel* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeLabel;
   };
/* #line 4930 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaUnionTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaUnionTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4945 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTypePtrList > storageOf_type_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaUnionType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaUnionType;
   };
/* #line 4965 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRvalueReferenceTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRvalueReferenceTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 4980 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRvalueReferenceType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRvalueReferenceType;
   };
/* #line 5000 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeNullptrStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeNullptrStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5015 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeNullptr* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeNullptr;
   };
/* #line 5034 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeclTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeclTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5049 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_expression;
     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeclType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeclType;
   };
/* #line 5070 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeOfTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeOfTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5085 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_expression;
     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeOfType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeOfType;
   };
/* #line 5106 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeMatrixStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeMatrixStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5121 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeMatrix* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeMatrix;
   };
/* #line 5141 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeTupleStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeTupleStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5156 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTypePtrList > storageOf_types;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeTuple* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeTuple;
   };
/* #line 5176 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeChar16StorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeChar16StorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5191 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeChar16* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeChar16;
   };
/* #line 5210 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeChar32StorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeChar32StorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5225 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeChar32* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeChar32;
   };
/* #line 5244 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeFloat128StorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeFloat128StorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5259 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeFloat128* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeFloat128;
   };
/* #line 5278 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeFixedStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeFixedStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5293 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_scale;
     unsigned long storageOf_fraction;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeFixed* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeFixed;
   };
/* #line 5314 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAutoTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAutoTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5329 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAutoType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAutoType;
   };
/* #line 5348 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaAccessTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaAccessTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5363 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaAccessType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaAccessType;
   };
/* #line 5383 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaSubtypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaSubtypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5398 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
     unsigned long storageOf_constraint;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaSubtype* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaSubtype;
   };
/* #line 5419 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaFloatTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaFloatTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5434 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_digits;
     unsigned long storageOf_range;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaFloatType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaFloatType;
   };
/* #line 5455 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialBitTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialBitTypeStorageClass  : public SgTypeStorageClass
   {

    protected: 


/* #line 5470 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialBitType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialBitType;
   };
/* #line 5490 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLocatedNodeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLocatedNodeStorageClass  : public SgNodeStorageClass
   {

    protected: 


/* #line 5505 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_startOfConstruct;
     unsigned long storageOf_endOfConstruct;
       EasyStorage < AttachedPreprocessingInfoType* > storageOf_attachedPreprocessingInfoPtr;
       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;
      bool storageOf_containsTransformationToSurroundingWhitespace;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLocatedNode* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLocatedNode;
   };
/* #line 5529 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTokenStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTokenStorageClass  : public SgLocatedNodeStorageClass
   {

    protected: 


/* #line 5544 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_lexeme_string;
      unsigned int storageOf_classification_code;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgToken* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgToken;
   };
/* #line 5565 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLocatedNodeSupportStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLocatedNodeSupportStorageClass  : public SgLocatedNodeStorageClass
   {

    protected: 


/* #line 5580 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLocatedNodeSupport* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLocatedNodeSupport;
   };
/* #line 5599 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCommonBlockObjectStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCommonBlockObjectStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5614 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_block_name;
     unsigned long storageOf_variable_reference_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCommonBlockObject* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCommonBlockObject;
   };
/* #line 5635 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInitializedNameStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInitializedNameStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5650 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
       EasyStorage < std::string > storageOf_microsoft_uuid_string;
     unsigned long storageOf_typeptr;
     unsigned long storageOf_initptr;
     unsigned long storageOf_prev_decl_item;
      bool storageOf_is_initializer;
     unsigned long storageOf_declptr;
     unsigned long storageOf_storageModifier;
     unsigned long storageOf_scope;
      SgInitializedName::preinitialization_enum storageOf_preinitialization;
      bool storageOf_isCoArray;
      SgInitializedName::asm_register_name_enum storageOf_register_name_code;
      SgInitializedName::excess_specifier_enum storageOf_excess_specifier;
       EasyStorage < std::string > storageOf_register_name_string;
      bool storageOf_requiresGlobalNameQualificationOnType;
      bool storageOf_shapeDeferred;
      bool storageOf_initializationDeferred;
       EasyStorage < SgBitVector > storageOf_gnu_attribute_modifierVector;
      unsigned long int storageOf_gnu_attribute_initialization_priority;
       EasyStorage < std::string > storageOf_gnu_attribute_named_weak_reference;
       EasyStorage < std::string > storageOf_gnu_attribute_named_alias;
       EasyStorage < std::string > storageOf_gnu_attribute_cleanup_function;
       EasyStorage < std::string > storageOf_gnu_attribute_section_name;
      int storageOf_gnu_attribute_alignment;
      SgDeclarationModifier::gnu_declaration_visability_enum storageOf_gnu_attribute_visability;
      bool storageOf_protected_declaration;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      int storageOf_name_qualification_length_for_type;
      bool storageOf_type_elaboration_required_for_type;
      bool storageOf_global_qualification_required_for_type;
      bool storageOf_hasArrayTypeWithEmptyBracketSyntax;
      bool storageOf_using_C11_Alignas_keyword;
     unsigned long storageOf_constant_or_type_argument_for_Alignas_keyword;
      bool storageOf_using_auto_keyword;
     unsigned long storageOf_auto_decltype;
      bool storageOf_using_device_keyword;
      bool storageOf_is_braced_initialized;
      bool storageOf_using_assignment_copy_constructor_syntax;
      bool storageOf_needs_definitions;
      bool storageOf_is_parameter_pack;
      bool storageOf_is_pack_element;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInitializedName* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInitializedName;
   };
/* #line 5712 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInterfaceBodyStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInterfaceBodyStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5727 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_function_name;
     unsigned long storageOf_functionDeclaration;
      bool storageOf_use_function_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInterfaceBody* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInterfaceBody;
   };
/* #line 5749 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgHeaderFileBodyStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgHeaderFileBodyStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5764 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_include_file;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgHeaderFileBody* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgHeaderFileBody;
   };
/* #line 5784 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRenamePairStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRenamePairStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5799 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_local_name;
      SgNameStorageClass storageOf_use_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRenamePair* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRenamePair;
   };
/* #line 5820 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaMemberValuePairStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaMemberValuePairStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5835 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaMemberValuePair* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaMemberValuePair;
   };
/* #line 5856 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpClauseStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 5871 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpClause;
   };
/* #line 5890 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpNowaitClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpNowaitClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 5905 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpNowaitClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpNowaitClause;
   };
/* #line 5924 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpBeginClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpBeginClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 5939 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpBeginClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpBeginClause;
   };
/* #line 5958 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpEndClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpEndClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 5973 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpEndClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpEndClause;
   };
/* #line 5992 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpUntiedClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpUntiedClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6007 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpUntiedClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpUntiedClause;
   };
/* #line 6026 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpDefaultClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpDefaultClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6041 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_default_option_enum storageOf_data_sharing;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpDefaultClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpDefaultClause;
   };
/* #line 6061 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpAtomicClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpAtomicClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6076 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_atomic_clause_enum storageOf_atomicity;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpAtomicClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpAtomicClause;
   };
/* #line 6096 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpProcBindClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpProcBindClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6111 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_proc_bind_policy_enum storageOf_policy;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpProcBindClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpProcBindClause;
   };
/* #line 6131 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpExpressionClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpExpressionClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6146 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpExpressionClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpExpressionClause;
   };
/* #line 6166 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpOrderedClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpOrderedClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6181 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpOrderedClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpOrderedClause;
   };
/* #line 6200 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpCollapseClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpCollapseClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6215 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpCollapseClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpCollapseClause;
   };
/* #line 6234 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpIfClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpIfClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6249 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpIfClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpIfClause;
   };
/* #line 6268 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpNumThreadsClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpNumThreadsClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6283 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpNumThreadsClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpNumThreadsClause;
   };
/* #line 6302 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpDeviceClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpDeviceClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6317 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpDeviceClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpDeviceClause;
   };
/* #line 6336 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSafelenClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSafelenClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6351 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSafelenClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSafelenClause;
   };
/* #line 6370 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSimdlenClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSimdlenClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6385 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSimdlenClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSimdlenClause;
   };
/* #line 6404 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpFinalClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpFinalClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6419 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpFinalClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpFinalClause;
   };
/* #line 6438 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpPriorityClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpPriorityClauseStorageClass  : public SgOmpExpressionClauseStorageClass
   {

    protected: 


/* #line 6453 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpPriorityClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpPriorityClause;
   };
/* #line 6472 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpInbranchClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpInbranchClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6487 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpInbranchClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpInbranchClause;
   };
/* #line 6506 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpNotinbranchClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpNotinbranchClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6521 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpNotinbranchClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpNotinbranchClause;
   };
/* #line 6540 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpVariablesClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpVariablesClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 6555 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpVariablesClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpVariablesClause;
   };
/* #line 6575 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpCopyprivateClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpCopyprivateClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6590 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpCopyprivateClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpCopyprivateClause;
   };
/* #line 6609 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpPrivateClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpPrivateClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6624 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpPrivateClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpPrivateClause;
   };
/* #line 6643 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpFirstprivateClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpFirstprivateClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6658 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpFirstprivateClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpFirstprivateClause;
   };
/* #line 6677 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSharedClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSharedClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6692 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSharedClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSharedClause;
   };
/* #line 6711 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpCopyinClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpCopyinClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6726 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpCopyinClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpCopyinClause;
   };
/* #line 6745 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpLastprivateClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpLastprivateClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6760 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpLastprivateClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpLastprivateClause;
   };
/* #line 6779 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpReductionClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpReductionClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6794 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_reduction_operator_enum storageOf_operation;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpReductionClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpReductionClause;
   };
/* #line 6814 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpMapClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpMapClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6829 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_map_operator_enum storageOf_operation;
       EasyStorage < std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > > > storageOf_array_dimensions;
       EasyStorage < std::map<SgSymbol*,  std::vector < std::pair <SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > > storageOf_dist_data_policies;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpMapClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpMapClause;
   };
/* #line 6851 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpUniformClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpUniformClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6866 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpUniformClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpUniformClause;
   };
/* #line 6885 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpAlignedClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpAlignedClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6900 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_alignment;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpAlignedClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpAlignedClause;
   };
/* #line 6920 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpLinearClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpLinearClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6935 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_step;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpLinearClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpLinearClause;
   };
/* #line 6955 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpDependClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpDependClauseStorageClass  : public SgOmpVariablesClauseStorageClass
   {

    protected: 


/* #line 6970 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_dependence_type_enum storageOf_dependence_type;
       EasyStorage < std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > > > storageOf_array_dimensions;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpDependClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpDependClause;
   };
/* #line 6991 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpScheduleClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpScheduleClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 7006 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgOmpClause::omp_schedule_kind_enum storageOf_kind;
     unsigned long storageOf_chunk_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpScheduleClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpScheduleClause;
   };
/* #line 7027 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpMergeableClauseStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpMergeableClauseStorageClass  : public SgOmpClauseStorageClass
   {

    protected: 


/* #line 7042 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpMergeableClause* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpMergeableClause;
   };
/* #line 7061 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNodeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNodeStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 7076 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNode* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNode;
   };
/* #line 7095 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExpressionStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 7110 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_expression_enum;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExpression;
   };
/* #line 7130 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedUnaryOperatorStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedUnaryOperatorStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7145 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_operator_name;
     unsigned long storageOf_operand;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedUnaryOperator* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedUnaryOperator;
   };
/* #line 7166 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedBinaryOperatorStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedBinaryOperatorStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7181 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_operator_name;
     unsigned long storageOf_lhs_operand;
     unsigned long storageOf_rhs_operand;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedBinaryOperator* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedBinaryOperator;
   };
/* #line 7203 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExprListExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExprListExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7218 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedExpressionPtrList > storageOf_expressions;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExprListExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExprListExpression;
   };
/* #line 7238 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedValueExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedValueExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7253 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_value_string;
     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedValueExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedValueExpression;
   };
/* #line 7274 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedArrayReferenceExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedArrayReferenceExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7289 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_array_subscripts;
     unsigned long storageOf_coarray_subscripts;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedArrayReferenceExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedArrayReferenceExpression;
   };
/* #line 7311 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedOtherExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedOtherExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7326 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedOtherExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedOtherExpression;
   };
/* #line 7345 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFunctionCallOrArrayReferenceExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFunctionCallOrArrayReferenceExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7360 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFunctionCallOrArrayReferenceExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFunctionCallOrArrayReferenceExpression;
   };
/* #line 7379 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedSubscriptExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedSubscriptExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7394 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lower_bound;
     unsigned long storageOf_upper_bound;
     unsigned long storageOf_stride;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedSubscriptExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedSubscriptExpression;
   };
/* #line 7416 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNamedExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNamedExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7431 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_expression_name;
     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNamedExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNamedExpression;
   };
/* #line 7452 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNullExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNullExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7467 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNullExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNullExpression;
   };
/* #line 7486 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedReferenceExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedReferenceExpressionStorageClass  : public SgUntypedExpressionStorageClass
   {

    protected: 


/* #line 7501 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedReferenceExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedReferenceExpression;
   };
/* #line 7521 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedStatementStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 7536 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_label_string;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedStatement;
   };
/* #line 7556 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedDeclarationStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 7571 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedDeclarationStatement;
   };
/* #line 7590 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNullDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNullDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7605 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNullDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNullDeclaration;
   };
/* #line 7624 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNameListDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNameListDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7639 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_names;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNameListDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNameListDeclaration;
   };
/* #line 7660 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedUseStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedUseStatementStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7675 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_module_name;
      SgToken::ROSE_Fortran_Keywords storageOf_module_nature;
     unsigned long storageOf_rename_list;
      bool storageOf_isOnlyList;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedUseStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedUseStatement;
   };
/* #line 7698 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedImplicitDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedImplicitDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7713 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedImplicitDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedImplicitDeclaration;
   };
/* #line 7732 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedVariableDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedVariableDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7747 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type;
     unsigned long storageOf_base_type_declaration;
      bool storageOf_has_base_type;
     unsigned long storageOf_modifiers;
     unsigned long storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedVariableDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedVariableDeclaration;
   };
/* #line 7771 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFunctionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFunctionDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7786 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_parameters;
     unsigned long storageOf_type;
     unsigned long storageOf_scope;
     unsigned long storageOf_modifiers;
     unsigned long storageOf_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFunctionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFunctionDeclaration;
   };
/* #line 7811 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedProgramHeaderDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedProgramHeaderDeclarationStorageClass  : public SgUntypedFunctionDeclarationStorageClass
   {

    protected: 


/* #line 7826 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedProgramHeaderDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedProgramHeaderDeclaration;
   };
/* #line 7845 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedSubroutineDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedSubroutineDeclarationStorageClass  : public SgUntypedFunctionDeclarationStorageClass
   {

    protected: 


/* #line 7860 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedSubroutineDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedSubroutineDeclaration;
   };
/* #line 7879 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedInterfaceDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedInterfaceDeclarationStorageClass  : public SgUntypedFunctionDeclarationStorageClass
   {

    protected: 


/* #line 7894 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedInterfaceDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedInterfaceDeclaration;
   };
/* #line 7914 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedModuleDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedModuleDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7929 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_scope;
     unsigned long storageOf_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedModuleDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedModuleDeclaration;
   };
/* #line 7951 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedSubmoduleDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedSubmoduleDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 7966 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
       EasyStorage < std::string > storageOf_submodule_ancestor;
       EasyStorage < std::string > storageOf_submodule_parent;
     unsigned long storageOf_scope;
     unsigned long storageOf_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedSubmoduleDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedSubmoduleDeclaration;
   };
/* #line 7990 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedBlockDataDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedBlockDataDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8005 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
      bool storageOf_has_name;
     unsigned long storageOf_declaration_list;
     unsigned long storageOf_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedBlockDataDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedBlockDataDeclaration;
   };
/* #line 8028 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedPackageDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedPackageDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8043 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedPackageDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedPackageDeclaration;
   };
/* #line 8064 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedStructureDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedStructureDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8079 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_modifiers;
     unsigned long storageOf_dim_info;
     unsigned long storageOf_definition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedStructureDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedStructureDeclaration;
   };
/* #line 8103 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExceptionHandlerDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExceptionHandlerDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8118 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExceptionHandlerDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExceptionHandlerDeclaration;
   };
/* #line 8138 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExceptionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExceptionDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8153 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExceptionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExceptionDeclaration;
   };
/* #line 8173 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTaskDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTaskDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8188 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTaskDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTaskDeclaration;
   };
/* #line 8209 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedUnitDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedUnitDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8224 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedUnitDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedUnitDeclaration;
   };
/* #line 8245 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedDirectiveDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedDirectiveDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8260 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
       EasyStorage < std::string > storageOf_directive_string;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedDirectiveDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedDirectiveDeclaration;
   };
/* #line 8281 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedEnumDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedEnumDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8296 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_enum_name;
     unsigned long storageOf_enumerators;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedEnumDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedEnumDeclaration;
   };
/* #line 8317 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTypedefDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTypedefDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8332 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_base_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTypedefDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTypedefDeclaration;
   };
/* #line 8353 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedInitializedNameListDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedInitializedNameListDeclarationStorageClass  : public SgUntypedDeclarationStatementStorageClass
   {

    protected: 


/* #line 8368 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedInitializedNameListDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedInitializedNameListDeclaration;
   };
/* #line 8389 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedAssignmentStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedAssignmentStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8404 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lhs_operand;
     unsigned long storageOf_rhs_operand;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedAssignmentStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedAssignmentStatement;
   };
/* #line 8425 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedBlockStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedBlockStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8440 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedBlockStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedBlockStatement;
   };
/* #line 8460 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExpressionStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExpressionStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8475 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_statement_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExpressionStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExpressionStatement;
   };
/* #line 8496 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFunctionCallStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFunctionCallStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8511 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_function;
     unsigned long storageOf_args;
       EasyStorage < std::string > storageOf_abort_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFunctionCallStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFunctionCallStatement;
   };
/* #line 8534 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedImageControlStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedImageControlStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8549 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_variable;
     unsigned long storageOf_expression;
     unsigned long storageOf_status_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedImageControlStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedImageControlStatement;
   };
/* #line 8572 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNamedStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNamedStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8587 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
       EasyStorage < std::string > storageOf_statement_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNamedStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNamedStatement;
   };
/* #line 8608 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedOtherStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedOtherStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8623 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedOtherStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedOtherStatement;
   };
/* #line 8643 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedScopeStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8658 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration_list;
     unsigned long storageOf_statement_list;
     unsigned long storageOf_function_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedScope;
   };
/* #line 8680 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFunctionScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFunctionScopeStorageClass  : public SgUntypedScopeStorageClass
   {

    protected: 


/* #line 8695 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFunctionScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFunctionScope;
   };
/* #line 8714 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedModuleScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedModuleScopeStorageClass  : public SgUntypedScopeStorageClass
   {

    protected: 


/* #line 8729 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedModuleScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedModuleScope;
   };
/* #line 8748 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedGlobalScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedGlobalScopeStorageClass  : public SgUntypedScopeStorageClass
   {

    protected: 


/* #line 8763 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedGlobalScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedGlobalScope;
   };
/* #line 8782 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedStructureDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedStructureDefinitionStorageClass  : public SgUntypedScopeStorageClass
   {

    protected: 


/* #line 8797 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_type_name;
      bool storageOf_has_type_name;
      bool storageOf_has_body;
     unsigned long storageOf_initializer;
     unsigned long storageOf_modifiers;
     unsigned long storageOf_base_type;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedStructureDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedStructureDefinition;
   };
/* #line 8823 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNullStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNullStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8838 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNullStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNullStatement;
   };
/* #line 8857 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedIfStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedIfStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8872 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_conditional;
     unsigned long storageOf_true_body;
     unsigned long storageOf_false_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedIfStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedIfStatement;
   };
/* #line 8895 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedCaseStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedCaseStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8910 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_expression;
     unsigned long storageOf_body;
       EasyStorage < std::string > storageOf_case_construct_name;
      bool storageOf_has_fall_through;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedCaseStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedCaseStatement;
   };
/* #line 8934 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedLabelStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedLabelStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8949 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedLabelStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedLabelStatement;
   };
/* #line 8969 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedLoopStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedLoopStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 8984 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedLoopStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedLoopStatement;
   };
/* #line 9003 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedWhileStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedWhileStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9018 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedWhileStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedWhileStatement;
   };
/* #line 9039 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedForStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedForStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9054 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_initialization;
     unsigned long storageOf_bound;
     unsigned long storageOf_increment;
     unsigned long storageOf_body;
       EasyStorage < std::string > storageOf_do_construct_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedForStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedForStatement;
   };
/* #line 9079 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExitStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9094 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExitStatement;
   };
/* #line 9113 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedGotoStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedGotoStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9128 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_target_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedGotoStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedGotoStatement;
   };
/* #line 9148 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedProcedureCallStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedProcedureCallStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9163 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedProcedureCallStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedProcedureCallStatement;
   };
/* #line 9182 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedReturnStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedReturnStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9197 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedReturnStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedReturnStatement;
   };
/* #line 9217 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedExtendedReturnStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedExtendedReturnStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9232 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedExtendedReturnStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedExtendedReturnStatement;
   };
/* #line 9251 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedAcceptStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedAcceptStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9266 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedAcceptStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedAcceptStatement;
   };
/* #line 9285 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedEntryCallStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedEntryCallStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9300 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedEntryCallStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedEntryCallStatement;
   };
/* #line 9319 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedRequeueStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedRequeueStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9334 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedRequeueStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedRequeueStatement;
   };
/* #line 9353 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedDelayUntilStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedDelayUntilStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9368 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedDelayUntilStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedDelayUntilStatement;
   };
/* #line 9387 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedDelayRelativeStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedDelayRelativeStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9402 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedDelayRelativeStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedDelayRelativeStatement;
   };
/* #line 9421 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTerminateAlternativeStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTerminateAlternativeStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9436 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTerminateAlternativeStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTerminateAlternativeStatement;
   };
/* #line 9455 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedSelectiveAcceptStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedSelectiveAcceptStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9470 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedSelectiveAcceptStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedSelectiveAcceptStatement;
   };
/* #line 9489 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTimedEntryCallStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTimedEntryCallStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9504 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTimedEntryCallStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTimedEntryCallStatement;
   };
/* #line 9523 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedConditionalEntryCallStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedConditionalEntryCallStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9538 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedConditionalEntryCallStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedConditionalEntryCallStatement;
   };
/* #line 9557 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedAsynchronousSelectStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedAsynchronousSelectStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9572 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedAsynchronousSelectStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedAsynchronousSelectStatement;
   };
/* #line 9591 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedAbortStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedAbortStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9606 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedAbortStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedAbortStatement;
   };
/* #line 9625 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedRaiseStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedRaiseStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9640 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedRaiseStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedRaiseStatement;
   };
/* #line 9659 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedStopStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedStopStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9674 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedStopStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedStopStatement;
   };
/* #line 9694 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedCodeStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedCodeStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9709 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedCodeStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedCodeStatement;
   };
/* #line 9728 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedForAllStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedForAllStatementStorageClass  : public SgUntypedStatementStorageClass
   {

    protected: 


/* #line 9743 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_statement_enum;
     unsigned long storageOf_type;
     unsigned long storageOf_iterates;
     unsigned long storageOf_local;
     unsigned long storageOf_mask;
       EasyStorage < std::string > storageOf_do_construct_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedForAllStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedForAllStatement;
   };
/* #line 9768 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNameStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNameStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 9783 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedName* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedName;
   };
/* #line 9803 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTokenStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTokenStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 9818 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_lexeme_string;
      unsigned int storageOf_classification_code;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedToken* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedToken;
   };
/* #line 9839 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTokenPairStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTokenPairStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 9854 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_lexeme_string_1;
      unsigned int storageOf_classification_code_1;
       EasyStorage < std::string > storageOf_lexeme_string_2;
      unsigned int storageOf_classification_code_2;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTokenPair* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTokenPair;
   };
/* #line 9877 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTypeStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 9892 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_type_name;
     unsigned long storageOf_type_kind;
      bool storageOf_has_kind;
      bool storageOf_is_literal;
      bool storageOf_is_class;
      bool storageOf_is_intrinsic;
      bool storageOf_is_constant;
      bool storageOf_is_user_defined;
     unsigned long storageOf_char_length_expression;
       EasyStorage < std::string > storageOf_char_length_string;
      bool storageOf_char_length_is_string;
     unsigned long storageOf_modifiers;
      SgUntypedType::type_enum storageOf_type_enum_id;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedType;
   };
/* #line 9924 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedArrayTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedArrayTypeStorageClass  : public SgUntypedTypeStorageClass
   {

    protected: 


/* #line 9939 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_dim_info;
      int storageOf_rank;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedArrayType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedArrayType;
   };
/* #line 9960 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTableTypeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTableTypeStorageClass  : public SgUntypedTypeStorageClass
   {

    protected: 


/* #line 9975 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_base_type;
     unsigned long storageOf_dim_info;
      int storageOf_rank;
      SgUntypedType::type_enum storageOf_table_type_enum_id;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTableType* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTableType;
   };
/* #line 9998 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedAttributeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedAttributeStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10013 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgToken::ROSE_Fortran_Keywords storageOf_type_name;
       EasyStorage < std::string > storageOf_named_attribute;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedAttribute* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedAttribute;
   };
/* #line 10034 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedInitializedNameStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedInitializedNameStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10049 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type;
       EasyStorage < std::string > storageOf_name;
      bool storageOf_has_initializer;
     unsigned long storageOf_initializer;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedInitializedName* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedInitializedName;
   };
/* #line 10072 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFileStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFileStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10087 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFile* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFile;
   };
/* #line 10107 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedStatementListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedStatementListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10122 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedStatementPtrList > storageOf_stmt_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedStatementList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedStatementList;
   };
/* #line 10142 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedDeclarationStatementListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedDeclarationStatementListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10157 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedDeclarationStatementPtrList > storageOf_decl_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedDeclarationStatementList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedDeclarationStatementList;
   };
/* #line 10177 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedFunctionDeclarationListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedFunctionDeclarationListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10192 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedFunctionDeclarationPtrList > storageOf_func_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedFunctionDeclarationList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedFunctionDeclarationList;
   };
/* #line 10212 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedInitializedNameListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedInitializedNameListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10227 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedInitializedNamePtrList > storageOf_name_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedInitializedNameList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedInitializedNameList;
   };
/* #line 10247 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedNameListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedNameListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10262 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedNamePtrList > storageOf_name_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedNameList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedNameList;
   };
/* #line 10282 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTokenListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTokenListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10297 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedTokenPtrList > storageOf_token_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTokenList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTokenList;
   };
/* #line 10317 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUntypedTokenPairListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUntypedTokenPairListStorageClass  : public SgUntypedNodeStorageClass
   {

    protected: 


/* #line 10332 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgUntypedTokenPairPtrList > storageOf_token_pair_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUntypedTokenPairList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUntypedTokenPairList;
   };
/* #line 10352 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLambdaCaptureStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLambdaCaptureStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 10367 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_capture_variable;
     unsigned long storageOf_source_closure_variable;
     unsigned long storageOf_closure_variable;
      bool storageOf_capture_by_reference;
      bool storageOf_implicit;
      bool storageOf_pack_expansion;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLambdaCapture* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLambdaCapture;
   };
/* #line 10392 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLambdaCaptureListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLambdaCaptureListStorageClass  : public SgLocatedNodeSupportStorageClass
   {

    protected: 


/* #line 10407 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgLambdaCapturePtrList > storageOf_capture_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLambdaCaptureList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLambdaCaptureList;
   };
/* #line 10427 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStatementStorageClass  : public SgLocatedNodeStorageClass
   {

    protected: 


/* #line 10442 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_numeric_label;
      int storageOf_source_sequence_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStatement;
   };
/* #line 10463 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgScopeStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgScopeStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 10478 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol_table;
     unsigned long storageOf_type_table;
       EasyStorage < std::set<SgSymbol*> > storageOf_type_elaboration_list;
       EasyStorage < std::set<SgSymbol*> > storageOf_hidden_type_list;
       EasyStorage < std::set<SgSymbol*> > storageOf_hidden_declaration_list;
     unsigned long storageOf_pragma;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgScopeStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgScopeStatement;
   };
/* #line 10503 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGlobalStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGlobalStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10518 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDeclarationStatementPtrList > storageOf_declarations;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGlobal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGlobal;
   };
/* #line 10538 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBasicBlockStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBasicBlockStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10553 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgStatementPtrList > storageOf_statements;
       EasyStorage < std::string > storageOf_asm_function_body;
       EasyStorage < std::string > storageOf_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBasicBlock* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBasicBlock;
   };
/* #line 10575 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIfStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIfStmtStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10590 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_conditional;
     unsigned long storageOf_true_body;
     unsigned long storageOf_false_body;
      bool storageOf_is_if_constexpr_statement;
      bool storageOf_if_constexpr_value_known;
      bool storageOf_if_constexpr_value;
     unsigned long storageOf_else_numeric_label;
       EasyStorage < std::string > storageOf_string_label;
     unsigned long storageOf_end_numeric_label;
      bool storageOf_has_end_statement;
      bool storageOf_use_then_keyword;
      bool storageOf_is_else_if_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIfStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIfStmt;
   };
/* #line 10621 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgForStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgForStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10636 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_for_init_stmt;
     unsigned long storageOf_test;
     unsigned long storageOf_increment;
     unsigned long storageOf_loop_body;
     unsigned long storageOf_else_body;
       EasyStorage < std::string > storageOf_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgForStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgForStatement;
   };
/* #line 10661 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionDefinitionStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10676 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
       EasyStorage < std::map<SgNode*,int> > storageOf_scope_number_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionDefinition;
   };
/* #line 10697 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateFunctionDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateFunctionDefinitionStorageClass  : public SgFunctionDefinitionStorageClass
   {

    protected: 


/* #line 10712 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateFunctionDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateFunctionDefinition;
   };
/* #line 10731 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassDefinitionStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10746 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDeclarationStatementPtrList > storageOf_members;
       EasyStorage < SgBaseClassPtrList > storageOf_inheritances;
      unsigned int storageOf_packingAlignment;
      bool storageOf_isSequence;
      bool storageOf_isPrivate;
      bool storageOf_isAbstract;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassDefinition;
   };
/* #line 10771 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationDefnStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationDefnStorageClass  : public SgClassDefinitionStorageClass
   {

    protected: 


/* #line 10786 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationDefn* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationDefn;
   };
/* #line 10805 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateClassDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateClassDefinitionStorageClass  : public SgClassDefinitionStorageClass
   {

    protected: 


/* #line 10820 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateClassDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateClassDefinition;
   };
/* #line 10839 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWhileStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWhileStmtStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10854 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
     unsigned long storageOf_body;
     unsigned long storageOf_else_body;
     unsigned long storageOf_end_numeric_label;
       EasyStorage < std::string > storageOf_string_label;
      bool storageOf_has_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWhileStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWhileStmt;
   };
/* #line 10879 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDoWhileStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDoWhileStmtStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10894 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
     unsigned long storageOf_condition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDoWhileStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDoWhileStmt;
   };
/* #line 10915 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSwitchStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSwitchStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10930 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_item_selector;
     unsigned long storageOf_body;
     unsigned long storageOf_end_numeric_label;
       EasyStorage < std::string > storageOf_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSwitchStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSwitchStatement;
   };
/* #line 10953 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCatchOptionStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCatchOptionStmtStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 10968 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
     unsigned long storageOf_body;
     unsigned long storageOf_trystmt;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCatchOptionStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCatchOptionStmt;
   };
/* #line 10990 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamespaceDefinitionStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamespaceDefinitionStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11005 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDeclarationStatementPtrList > storageOf_declarations;
     unsigned long storageOf_namespaceDeclaration;
     unsigned long storageOf_previousNamespaceDefinition;
     unsigned long storageOf_nextNamespaceDefinition;
     unsigned long storageOf_global_definition;
      bool storageOf_isUnionOfReentrantNamespaceDefinitions;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamespaceDefinitionStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamespaceDefinitionStatement;
   };
/* #line 11030 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBlockDataStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBlockDataStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11045 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBlockDataStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBlockDataStatement;
   };
/* #line 11065 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssociateStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssociateStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11080 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDeclarationStatementPtrList > storageOf_associates;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssociateStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssociateStatement;
   };
/* #line 11101 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFortranDoStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFortranDoStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11116 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_initialization;
     unsigned long storageOf_bound;
     unsigned long storageOf_increment;
     unsigned long storageOf_body;
     unsigned long storageOf_end_numeric_label;
       EasyStorage < std::string > storageOf_string_label;
      bool storageOf_old_style;
      bool storageOf_has_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFortranDo* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFortranDo;
   };
/* #line 11143 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFortranNonblockedDoStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFortranNonblockedDoStorageClass  : public SgFortranDoStorageClass
   {

    protected: 


/* #line 11158 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFortranNonblockedDo* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFortranNonblockedDo;
   };
/* #line 11178 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgForAllStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgForAllStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11193 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_forall_header;
     unsigned long storageOf_body;
      bool storageOf_has_end_statement;
       EasyStorage < std::string > storageOf_string_label;
     unsigned long storageOf_end_numeric_label;
      SgForAllStatement::forall_statement_kind_enum storageOf_forall_statement_kind;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgForAllStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgForAllStatement;
   };
/* #line 11218 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcForAllStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcForAllStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11233 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_for_init_stmt;
     unsigned long storageOf_test;
     unsigned long storageOf_increment;
     unsigned long storageOf_affinity;
     unsigned long storageOf_loop_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcForAllStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcForAllStatement;
   };
/* #line 11257 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCAFWithTeamStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCAFWithTeamStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11272 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_teamId;
     unsigned long storageOf_body;
      bool storageOf_endHasTeamId;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCAFWithTeamStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCAFWithTeamStatement;
   };
/* #line 11294 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaForEachStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaForEachStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11309 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_element;
     unsigned long storageOf_collection;
     unsigned long storageOf_loop_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaForEachStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaForEachStatement;
   };
/* #line 11331 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaLabelStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaLabelStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11346 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_label;
     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaLabelStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaLabelStatement;
   };
/* #line 11367 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMatlabForStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMatlabForStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11382 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_index;
     unsigned long storageOf_range;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMatlabForStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMatlabForStatement;
   };
/* #line 11404 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionParameterScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionParameterScopeStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11419 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgDeclarationStatementPtrList > storageOf_declarations;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionParameterScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionParameterScope;
   };
/* #line 11439 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeclarationScopeStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeclarationScopeStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11454 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeclarationScope* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeclarationScope;
   };
/* #line 11473 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRangeBasedForStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRangeBasedForStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11488 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_iterator_declaration;
     unsigned long storageOf_range_declaration;
     unsigned long storageOf_begin_declaration;
     unsigned long storageOf_end_declaration;
     unsigned long storageOf_not_equal_expression;
     unsigned long storageOf_increment_expression;
     unsigned long storageOf_loop_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRangeBasedForStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRangeBasedForStatement;
   };
/* #line 11514 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialForThenStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialForThenStatementStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11529 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_initialization;
     unsigned long storageOf_while_expression;
     unsigned long storageOf_by_or_then_expression;
     unsigned long storageOf_loop_body;
      SgJovialForThenStatement::loop_statement_type_enum storageOf_loop_statement_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialForThenStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialForThenStatement;
   };
/* #line 11553 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaAcceptStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaAcceptStmtStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11568 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_parameterScope;
     unsigned long storageOf_parameterList;
     unsigned long storageOf_entry;
     unsigned long storageOf_index;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaAcceptStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaAcceptStmt;
   };
/* #line 11592 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaPackageSpecStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaPackageSpecStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11607 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
       EasyStorage < SgDeclarationStatementPtrList > storageOf_declarations;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaPackageSpec* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaPackageSpec;
   };
/* #line 11628 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaPackageBodyStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaPackageBodyStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11643 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_spec;
       EasyStorage < SgStatementPtrList > storageOf_statements;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaPackageBody* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaPackageBody;
   };
/* #line 11664 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskSpecStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskSpecStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11679 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
      bool storageOf_hasMembers;
       EasyStorage < SgDeclarationStatementPtrList > storageOf_declarations;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskSpec* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskSpec;
   };
/* #line 11701 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskBodyStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskBodyStorageClass  : public SgScopeStatementStorageClass
   {

    protected: 


/* #line 11716 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_spec;
       EasyStorage < SgStatementPtrList > storageOf_statements;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskBody* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskBody;
   };
/* #line 11737 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionTypeTableStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionTypeTableStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 11752 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_function_type_table;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionTypeTable* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionTypeTable;
   };
/* #line 11772 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeclarationStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 11787 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned int storageOf_decl_attributes;
       EasyStorage < std::string > storageOf_linkage;
      SgDeclarationModifierStorageClass storageOf_declarationModifier;
      bool storageOf_nameOnly;
      bool storageOf_forward;
      bool storageOf_externBrace;
      bool storageOf_skipElaborateType;
     unsigned long storageOf_definingDeclaration;
     unsigned long storageOf_firstNondefiningDeclaration;
       EasyStorage < SgQualifiedNamePtrList > storageOf_qualifiedNameList;
       EasyStorage < std::string > storageOf_binding_label;
     unsigned long storageOf_declarationScope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeclarationStatement;
   };
/* #line 11818 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionParameterListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionParameterListStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 11833 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgInitializedNamePtrList > storageOf_args;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionParameterList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionParameterList;
   };
/* #line 11853 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVariableDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVariableDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 11868 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_baseTypeDefiningDeclaration;
       EasyStorage < SgInitializedNamePtrList > storageOf_variables;
      bool storageOf_variableDeclarationContainsBaseTypeDefiningDeclaration;
      SgDeclarationStatement::template_specialization_enum storageOf_specialization;
      bool storageOf_requiresGlobalNameQualificationOnType;
       EasyStorage < std::string > storageOf_gnu_extension_section;
       EasyStorage < std::string > storageOf_gnu_extension_alias;
      unsigned short storageOf_gnu_extension_initialization_priority;
      SgDeclarationStatement::gnu_extension_visability_attribute_enum storageOf_gnu_extension_visability;
      SgVariableDeclaration::gnu_extension_declaration_attributes_enum storageOf_gnu_extension_declaration_attribute;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_isAssociatedWithDeclarationList;
      bool storageOf_isFirstDeclarationOfDeclarationList;
      bool storageOf_is_thread_local;
      bool storageOf_is_constexpr;
      bool storageOf_builtFromUseOnly;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVariableDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVariableDeclaration;
   };
/* #line 11905 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateVariableDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateVariableDeclarationStorageClass  : public SgVariableDeclarationStorageClass
   {

    protected: 


/* #line 11920 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateSpecializationArguments;
      SgNameStorageClass storageOf_string;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateVariableDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateVariableDeclaration;
   };
/* #line 11943 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVariableDefinitionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVariableDefinitionStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 11958 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_vardefn;
     unsigned long storageOf_bitfield;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVariableDefinition* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVariableDefinition;
   };
/* #line 11979 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClinkageDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClinkageDeclarationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 11994 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_languageSpecifier;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClinkageDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClinkageDeclarationStatement;
   };
/* #line 12014 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClinkageStartStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClinkageStartStatementStorageClass  : public SgClinkageDeclarationStatementStorageClass
   {

    protected: 


/* #line 12029 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClinkageStartStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClinkageStartStatement;
   };
/* #line 12048 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClinkageEndStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClinkageEndStatementStorageClass  : public SgClinkageDeclarationStatementStorageClass
   {

    protected: 


/* #line 12063 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClinkageEndStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClinkageEndStatement;
   };
/* #line 12082 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEnumDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEnumDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12097 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      bool storageOf_embedded;
     unsigned long storageOf_type;
       EasyStorage < SgInitializedNamePtrList > storageOf_enumerators;
     unsigned long storageOf_scope;
      bool storageOf_isUnNamed;
      bool storageOf_isAutonomousDeclaration;
     unsigned long storageOf_field_type;
      bool storageOf_isScopedEnum;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEnumDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEnumDeclaration;
   };
/* #line 12128 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAsmStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAsmStmtStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12143 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_assemblyCode;
      bool storageOf_useGnuExtendedFormat;
       EasyStorage < SgExpressionPtrList > storageOf_operands;
       EasyStorage < SgAsmStmt::AsmRegisterNameList > storageOf_clobberRegisterList;
      bool storageOf_isVolatile;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAsmStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAsmStmt;
   };
/* #line 12167 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAttributeSpecificationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAttributeSpecificationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12182 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgStringList > storageOf_name_list;
      SgAttributeSpecificationStatement::attribute_spec_enum storageOf_attribute_kind;
      int storageOf_intent;
     unsigned long storageOf_parameter_list;
       EasyStorage < SgDataStatementGroupPtrList > storageOf_data_statement_group_list;
     unsigned long storageOf_bind_list;
       EasyStorage < SgDimensionObjectPtrList > storageOf_dimension_object_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAttributeSpecificationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAttributeSpecificationStatement;
   };
/* #line 12208 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFormatStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFormatStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12223 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_format_item_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFormatStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFormatStatement;
   };
/* #line 12243 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12258 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      SgNameStorageClass storageOf_string;
      SgTemplateDeclaration::template_type_enum storageOf_template_kind;
       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
     unsigned long storageOf_scope;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateDeclaration;
   };
/* #line 12283 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationDirectiveStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12298 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;
      bool storageOf_do_not_instantiate;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationDirectiveStatement;
   };
/* #line 12319 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUseStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUseStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12334 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      bool storageOf_only_option;
       EasyStorage < SgRenamePairPtrList > storageOf_rename_list;
     unsigned long storageOf_module;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUseStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUseStatement;
   };
/* #line 12357 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgParameterStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgParameterStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12372 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgParameterStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgParameterStatement;
   };
/* #line 12391 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamespaceDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamespaceDeclarationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12406 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;
      bool storageOf_isUnnamedNamespace;
      bool storageOf_isInlinedNamespace;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamespaceDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamespaceDeclarationStatement;
   };
/* #line 12429 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEquivalenceStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEquivalenceStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12444 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_equivalence_set_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEquivalenceStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEquivalenceStatement;
   };
/* #line 12464 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInterfaceStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInterfaceStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12479 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      SgInterfaceStatement::generic_spec_enum storageOf_generic_spec;
       EasyStorage < SgInterfaceBodyPtrList > storageOf_interface_body_list;
     unsigned long storageOf_end_numeric_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInterfaceStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInterfaceStatement;
   };
/* #line 12502 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamespaceAliasDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamespaceAliasDeclarationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12517 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_namespaceDeclaration;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_is_alias_for_another_namespace_alias;
     unsigned long storageOf_namespaceAliasDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamespaceAliasDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamespaceAliasDeclarationStatement;
   };
/* #line 12543 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCommonBlockStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCommonBlockStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12558 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgCommonBlockObjectPtrList > storageOf_block_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCommonBlock* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCommonBlock;
   };
/* #line 12578 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypedefDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypedefDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12593 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_base_type;
     unsigned long storageOf_type;
     unsigned long storageOf_declaration;
     unsigned long storageOf_parent_scope;
      bool storageOf_typedefBaseTypeContainsDefiningDeclaration;
     unsigned long storageOf_scope;
      bool storageOf_requiresGlobalNameQualificationOnType;
      int storageOf_name_qualification_length_for_base_type;
      bool storageOf_type_elaboration_required_for_base_type;
      bool storageOf_global_qualification_required_for_base_type;
      bool storageOf_isAutonomousDeclaration;
      bool storageOf_isAssociatedWithDeclarationList;
      bool storageOf_isFirstDeclarationOfDeclarationList;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypedefDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypedefDeclaration;
   };
/* #line 12629 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateTypedefDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateTypedefDeclarationStorageClass  : public SgTypedefDeclarationStorageClass
   {

    protected: 


/* #line 12644 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateSpecializationArguments;
      SgNameStorageClass storageOf_string;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateTypedefDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateTypedefDeclaration;
   };
/* #line 12667 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationTypedefDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationTypedefDeclarationStorageClass  : public SgTypedefDeclarationStorageClass
   {

    protected: 


/* #line 12682 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_templateName;
      SgNameStorageClass storageOf_templateHeader;
     unsigned long storageOf_templateDeclaration;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateArguments;
      bool storageOf_nameResetFromMangledForm;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationTypedefDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationTypedefDeclaration;
   };
/* #line 12706 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStatementFunctionStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStatementFunctionStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12721 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_function;
     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStatementFunctionStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStatementFunctionStatement;
   };
/* #line 12742 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCtorInitializerListStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCtorInitializerListStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12757 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgInitializedNamePtrList > storageOf_ctors;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCtorInitializerList* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCtorInitializerList;
   };
/* #line 12777 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPragmaDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPragmaDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12792 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_pragma;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPragmaDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPragmaDeclaration;
   };
/* #line 12812 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUsingDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUsingDirectiveStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12827 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_namespaceDeclaration;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUsingDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUsingDirectiveStatement;
   };
/* #line 12850 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 12865 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
      SgClassDeclaration::class_types storageOf_class_type;
     unsigned long storageOf_type;
     unsigned long storageOf_definition;
     unsigned long storageOf_scope;
      SgDeclarationStatement::template_specialization_enum storageOf_specialization;
      bool storageOf_from_template;
      bool storageOf_fixupScope;
      bool storageOf_isUnNamed;
      bool storageOf_explicit_annotation_interface;
      bool storageOf_explicit_interface;
      bool storageOf_explicit_enum;
      bool storageOf_explicit_anonymous;
     unsigned long storageOf_decoratorList;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_isAutonomousDeclaration;
      bool storageOf_isRepresentingTemplateParameterInTemplateDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassDeclaration;
   };
/* #line 12903 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateClassDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateClassDeclarationStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 12918 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateSpecializationArguments;
      SgNameStorageClass storageOf_string;
      SgNameStorageClass storageOf_templateName;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateClassDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateClassDeclaration;
   };
/* #line 12942 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationDeclStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 12957 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_templateName;
      SgNameStorageClass storageOf_templateHeader;
     unsigned long storageOf_templateDeclaration;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateArguments;
      bool storageOf_nameResetFromMangledForm;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationDecl;
   };
/* #line 12981 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDerivedTypeStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDerivedTypeStatementStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 12996 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_end_numeric_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDerivedTypeStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDerivedTypeStatement;
   };
/* #line 13016 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModuleStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModuleStatementStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 13031 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_end_numeric_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModuleStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModuleStatement;
   };
/* #line 13051 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaPackageDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaPackageDeclarationStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 13066 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaPackageDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaPackageDeclaration;
   };
/* #line 13085 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialTableStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialTableStatementStorageClass  : public SgClassDeclarationStorageClass
   {

    protected: 


/* #line 13100 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_table_entry_size;
      bool storageOf_has_table_entry_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialTableStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialTableStatement;
   };
/* #line 13121 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgImplicitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgImplicitStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13136 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_implicit_none;
      SgImplicitStatement::implicit_spec_enum storageOf_implicit_spec;
       EasyStorage < SgInitializedNamePtrList > storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgImplicitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgImplicitStatement;
   };
/* #line 13158 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUsingDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUsingDeclarationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13173 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;
     unsigned long storageOf_initializedName;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_is_inheriting_constructor;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUsingDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUsingDeclarationStatement;
   };
/* #line 13198 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamelistStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamelistStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13213 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgNameGroupPtrList > storageOf_group_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamelistStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamelistStatement;
   };
/* #line 13233 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgImportStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgImportStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13248 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgExpressionPtrList > storageOf_import_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgImportStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgImportStatement;
   };
/* #line 13268 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13283 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_parameterList;
      SgFunctionModifierStorageClass storageOf_functionModifier;
      SgSpecialFunctionModifierStorageClass storageOf_specialFunctionModifier;
       EasyStorage < SgTypePtrList > storageOf_exceptionSpecification;
      bool storageOf_named_in_end_statement;
       EasyStorage < std::string > storageOf_asm_name;
     unsigned long storageOf_decoratorList;
     unsigned long storageOf_type;
     unsigned long storageOf_definition;
      bool storageOf_oldStyleDefinition;
     unsigned long storageOf_scope;
      SgDeclarationStatement::template_specialization_enum storageOf_specialization;
      bool storageOf_requiresNameQualificationOnReturnType;
       EasyStorage < std::string > storageOf_gnu_extension_section;
       EasyStorage < std::string > storageOf_gnu_extension_alias;
      SgDeclarationStatement::gnu_extension_visability_attribute_enum storageOf_gnu_extension_visability;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      int storageOf_name_qualification_length_for_return_type;
      bool storageOf_type_elaboration_required_for_return_type;
      bool storageOf_global_qualification_required_for_return_type;
      bool storageOf_prototypeIsWithoutParameters;
      int storageOf_gnu_regparm_attribute;
     unsigned long storageOf_type_syntax;
      bool storageOf_type_syntax_is_available;
     unsigned long storageOf_parameterList_syntax;
      bool storageOf_using_C11_Noreturn_keyword;
      bool storageOf_is_constexpr;
      bool storageOf_using_new_function_return_type_syntax;
     unsigned long storageOf_functionParameterScope;
      bool storageOf_marked_as_edg_normalization;
      bool storageOf_is_implicit_function;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionDeclaration;
   };
/* #line 13336 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateFunctionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateFunctionDeclarationStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13351 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateSpecializationArguments;
      SgNameStorageClass storageOf_string;
      bool storageOf_string_represents_function_body;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateFunctionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateFunctionDeclaration;
   };
/* #line 13375 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMemberFunctionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMemberFunctionDeclarationStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13390 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_CtorInitializerList;
     unsigned long storageOf_associatedClassDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMemberFunctionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMemberFunctionDeclaration;
   };
/* #line 13411 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateMemberFunctionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateMemberFunctionDeclarationStorageClass  : public SgMemberFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13426 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgTemplateParameterPtrList > storageOf_templateParameters;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateSpecializationArguments;
      SgNameStorageClass storageOf_string;
      bool storageOf_string_represents_function_body;
     unsigned long storageOf_nonreal_decl_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateMemberFunctionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateMemberFunctionDeclaration;
   };
/* #line 13450 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationMemberFunctionDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationMemberFunctionDeclStorageClass  : public SgMemberFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13465 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_templateName;
     unsigned long storageOf_templateDeclaration;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateArguments;
      bool storageOf_nameResetFromMangledForm;
      bool storageOf_template_argument_list_is_explicit;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationMemberFunctionDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationMemberFunctionDecl;
   };
/* #line 13489 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateInstantiationFunctionDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateInstantiationFunctionDeclStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13504 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_templateName;
     unsigned long storageOf_templateDeclaration;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_templateArguments;
      bool storageOf_nameResetFromMangledForm;
      bool storageOf_template_argument_list_is_explicit;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateInstantiationFunctionDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateInstantiationFunctionDecl;
   };
/* #line 13528 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgProgramHeaderStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgProgramHeaderStatementStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13543 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_program_statement_explicit;
     unsigned long storageOf_end_numeric_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgProgramHeaderStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgProgramHeaderStatement;
   };
/* #line 13564 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgProcedureHeaderStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgProcedureHeaderStatementStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13579 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgProcedureHeaderStatement::subprogram_kind_enum storageOf_subprogram_kind;
     unsigned long storageOf_end_numeric_label;
     unsigned long storageOf_result_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgProcedureHeaderStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgProcedureHeaderStatement;
   };
/* #line 13601 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEntryStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEntryStatementStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13616 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_result_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEntryStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEntryStatement;
   };
/* #line 13636 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaEntryDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaEntryDeclStorageClass  : public SgFunctionDeclarationStorageClass
   {

    protected: 


/* #line 13651 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_discrete_index_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaEntryDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaEntryDecl;
   };
/* #line 13671 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgContainsStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgContainsStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13686 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgContainsStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgContainsStatement;
   };
/* #line 13705 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgC_PreprocessorDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgC_PreprocessorDirectiveStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 13720 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_directiveString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgC_PreprocessorDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgC_PreprocessorDirectiveStatement;
   };
/* #line 13740 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIncludeDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIncludeDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13755 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_headerFileBody;
     unsigned long storageOf_include_file_heirarchy;
       EasyStorage < std::string > storageOf_name_used_in_include_directive;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIncludeDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIncludeDirectiveStatement;
   };
/* #line 13777 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDefineDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDefineDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13792 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDefineDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDefineDirectiveStatement;
   };
/* #line 13811 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUndefDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUndefDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13826 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUndefDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUndefDirectiveStatement;
   };
/* #line 13845 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIfdefDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIfdefDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13860 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIfdefDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIfdefDirectiveStatement;
   };
/* #line 13879 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIfndefDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIfndefDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13894 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIfndefDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIfndefDirectiveStatement;
   };
/* #line 13913 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIfDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIfDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13928 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIfDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIfDirectiveStatement;
   };
/* #line 13947 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeadIfDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeadIfDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13962 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeadIfDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeadIfDirectiveStatement;
   };
/* #line 13981 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElseDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElseDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 13996 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElseDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElseDirectiveStatement;
   };
/* #line 14015 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElseifDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElseifDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14030 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElseifDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElseifDirectiveStatement;
   };
/* #line 14049 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEndifDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEndifDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14064 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEndifDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEndifDirectiveStatement;
   };
/* #line 14083 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLineDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLineDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14098 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLineDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLineDirectiveStatement;
   };
/* #line 14117 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWarningDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWarningDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14132 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWarningDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWarningDirectiveStatement;
   };
/* #line 14151 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgErrorDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgErrorDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14166 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgErrorDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgErrorDirectiveStatement;
   };
/* #line 14185 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEmptyDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEmptyDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14200 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEmptyDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEmptyDirectiveStatement;
   };
/* #line 14219 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIncludeNextDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIncludeNextDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14234 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIncludeNextDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIncludeNextDirectiveStatement;
   };
/* #line 14253 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIdentDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIdentDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14268 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIdentDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIdentDirectiveStatement;
   };
/* #line 14287 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLinemarkerDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLinemarkerDirectiveStatementStorageClass  : public SgC_PreprocessorDirectiveStatementStorageClass
   {

    protected: 


/* #line 14302 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_linenumber;
       EasyStorage < std::string > storageOf_filename;
       EasyStorage < SgUnsignedCharList > storageOf_flaglist;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLinemarkerDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLinemarkerDirectiveStatement;
   };
/* #line 14324 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpThreadprivateStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpThreadprivateStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14339 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgVarRefExpPtrList > storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpThreadprivateStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpThreadprivateStatement;
   };
/* #line 14359 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFortranIncludeLineStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFortranIncludeLineStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14374 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_filename;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFortranIncludeLine* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFortranIncludeLine;
   };
/* #line 14394 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaImportStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaImportStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14409 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_path;
      bool storageOf_containsWildCard;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaImportStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaImportStatement;
   };
/* #line 14430 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaPackageStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaPackageStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14445 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaPackageStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaPackageStatement;
   };
/* #line 14465 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStmtDeclarationStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStmtDeclarationStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14480 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStmtDeclarationStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStmtDeclarationStatement;
   };
/* #line 14500 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStaticAssertionDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStaticAssertionDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14515 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
      SgNameStorageClass storageOf_string_literal;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStaticAssertionDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStaticAssertionDeclaration;
   };
/* #line 14536 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpDeclareSimdStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpDeclareSimdStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14551 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgOmpClausePtrList > storageOf_clauses;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpDeclareSimdStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpDeclareSimdStatement;
   };
/* #line 14571 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMicrosoftAttributeDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMicrosoftAttributeDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14586 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_attribute_string;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMicrosoftAttributeDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMicrosoftAttributeDeclaration;
   };
/* #line 14606 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialCompoolStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialCompoolStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14621 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialCompoolStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialCompoolStatement;
   };
/* #line 14641 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialDirectiveStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialDirectiveStatementStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14656 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_content_string;
      SgJovialDirectiveStatement::directive_types storageOf_directive_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialDirectiveStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialDirectiveStatement;
   };
/* #line 14677 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialDefineDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialDefineDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14692 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_define_string;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialDefineDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialDefineDeclaration;
   };
/* #line 14712 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialOverlayDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialOverlayDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14727 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_address;
     unsigned long storageOf_overlay;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialOverlayDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialOverlayDeclaration;
   };
/* #line 14748 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonrealDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonrealDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14763 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_nonreal_decl_scope;
     unsigned long storageOf_type;
      int storageOf_template_parameter_position;
      int storageOf_template_parameter_depth;
     unsigned long storageOf_templateDeclaration;
       EasyStorage < SgTemplateArgumentPtrList > storageOf_tpl_args;
       EasyStorage < SgTemplateParameterPtrList > storageOf_tpl_params;
      bool storageOf_is_class_member;
      bool storageOf_is_template_param;
      bool storageOf_is_template_template_param;
      bool storageOf_is_nonreal_template;
      bool storageOf_is_nonreal_function;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonrealDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonrealDecl;
   };
/* #line 14795 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEmptyDeclarationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEmptyDeclarationStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14810 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEmptyDeclaration* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEmptyDeclaration;
   };
/* #line 14829 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaPackageBodyDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaPackageBodyDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14844 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaPackageBodyDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaPackageBodyDecl;
   };
/* #line 14866 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaPackageSpecDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaPackageSpecDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14881 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;
     unsigned long storageOf_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaPackageSpecDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaPackageSpecDecl;
   };
/* #line 14903 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaRenamingDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaRenamingDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14918 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_renamed;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaRenamingDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaRenamingDecl;
   };
/* #line 14939 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskSpecDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskSpecDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14954 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskSpecDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskSpecDecl;
   };
/* #line 14975 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskBodyDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskBodyDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 14990 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskBodyDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskBodyDecl;
   };
/* #line 15011 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskTypeDeclStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskTypeDeclStorageClass  : public SgDeclarationStatementStorageClass
   {

    protected: 


/* #line 15026 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_definition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskTypeDecl* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskTypeDecl;
   };
/* #line 15047 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExprStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExprStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15062 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExprStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExprStatement;
   };
/* #line 15082 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLabelStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLabelStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15097 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_label;
     unsigned long storageOf_scope;
     unsigned long storageOf_statement;
      bool storageOf_gnu_extension_unused;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLabelStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLabelStatement;
   };
/* #line 15120 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCaseOptionStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCaseOptionStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15135 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_key;
     unsigned long storageOf_body;
     unsigned long storageOf_key_range_end;
       EasyStorage < std::string > storageOf_case_construct_name;
      bool storageOf_has_fall_through;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCaseOptionStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCaseOptionStmt;
   };
/* #line 15159 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTryStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTryStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15174 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
     unsigned long storageOf_catch_statement_seq_root;
     unsigned long storageOf_else_body;
     unsigned long storageOf_finally_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTryStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTryStmt;
   };
/* #line 15197 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDefaultOptionStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDefaultOptionStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15212 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
       EasyStorage < std::string > storageOf_default_construct_name;
      bool storageOf_has_fall_through;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDefaultOptionStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDefaultOptionStmt;
   };
/* #line 15234 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBreakStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBreakStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15249 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_do_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBreakStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBreakStmt;
   };
/* #line 15269 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgContinueStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgContinueStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15284 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_do_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgContinueStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgContinueStmt;
   };
/* #line 15304 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgReturnStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgReturnStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15319 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgReturnStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgReturnStmt;
   };
/* #line 15339 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGotoStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGotoStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15354 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_label;
     unsigned long storageOf_label_expression;
     unsigned long storageOf_selector_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGotoStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGotoStatement;
   };
/* #line 15376 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSpawnStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSpawnStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15391 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_the_func;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSpawnStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSpawnStmt;
   };
/* #line 15411 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNullStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNullStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15426 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNullStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNullStatement;
   };
/* #line 15445 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVariantStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVariantStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15460 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVariantStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVariantStatement;
   };
/* #line 15479 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgForInitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgForInitStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15494 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgStatementPtrList > storageOf_init_stmt;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgForInitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgForInitStatement;
   };
/* #line 15514 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCatchStatementSeqStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCatchStatementSeqStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15529 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgStatementPtrList > storageOf_catch_statement_seq;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCatchStatementSeq* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCatchStatementSeq;
   };
/* #line 15549 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgProcessControlStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgProcessControlStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15564 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgProcessControlStatement::control_enum storageOf_control_kind;
     unsigned long storageOf_code;
     unsigned long storageOf_quiet;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgProcessControlStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgProcessControlStatement;
   };
/* #line 15586 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIOStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIOStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 15601 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgIOStatement::io_statement_enum storageOf_io_statement;
     unsigned long storageOf_io_stmt_list;
     unsigned long storageOf_unit;
     unsigned long storageOf_iostat;
     unsigned long storageOf_err;
     unsigned long storageOf_iomsg;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIOStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIOStatement;
   };
/* #line 15626 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPrintStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPrintStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15641 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_format;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPrintStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPrintStatement;
   };
/* #line 15661 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgReadStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgReadStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15676 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_format;
     unsigned long storageOf_namelist;
     unsigned long storageOf_advance;
     unsigned long storageOf_asynchronous;
     unsigned long storageOf_blank;
     unsigned long storageOf_decimal;
     unsigned long storageOf_delim;
     unsigned long storageOf_end;
     unsigned long storageOf_eor;
     unsigned long storageOf_id;
     unsigned long storageOf_pad;
     unsigned long storageOf_pos;
     unsigned long storageOf_rec;
     unsigned long storageOf_round;
     unsigned long storageOf_sign;
     unsigned long storageOf_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgReadStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgReadStatement;
   };
/* #line 15711 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWriteStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWriteStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15726 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_format;
     unsigned long storageOf_namelist;
     unsigned long storageOf_advance;
     unsigned long storageOf_asynchronous;
     unsigned long storageOf_blank;
     unsigned long storageOf_decimal;
     unsigned long storageOf_delim;
     unsigned long storageOf_end;
     unsigned long storageOf_eor;
     unsigned long storageOf_id;
     unsigned long storageOf_pad;
     unsigned long storageOf_pos;
     unsigned long storageOf_rec;
     unsigned long storageOf_round;
     unsigned long storageOf_sign;
     unsigned long storageOf_size;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWriteStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWriteStatement;
   };
/* #line 15761 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOpenStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOpenStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15776 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_file;
     unsigned long storageOf_status;
     unsigned long storageOf_access;
     unsigned long storageOf_form;
     unsigned long storageOf_recl;
     unsigned long storageOf_blank;
     unsigned long storageOf_position;
     unsigned long storageOf_action;
     unsigned long storageOf_delim;
     unsigned long storageOf_pad;
     unsigned long storageOf_round;
     unsigned long storageOf_sign;
     unsigned long storageOf_asynchronous;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOpenStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOpenStatement;
   };
/* #line 15808 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCloseStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCloseStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15823 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_status;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCloseStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCloseStatement;
   };
/* #line 15843 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInquireStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInquireStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15858 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_file;
     unsigned long storageOf_access;
     unsigned long storageOf_form;
     unsigned long storageOf_recl;
     unsigned long storageOf_blank;
     unsigned long storageOf_exist;
     unsigned long storageOf_opened;
     unsigned long storageOf_number;
     unsigned long storageOf_named;
     unsigned long storageOf_name;
     unsigned long storageOf_sequential;
     unsigned long storageOf_direct;
     unsigned long storageOf_formatted;
     unsigned long storageOf_unformatted;
     unsigned long storageOf_nextrec;
     unsigned long storageOf_position;
     unsigned long storageOf_action;
     unsigned long storageOf_read;
     unsigned long storageOf_write;
     unsigned long storageOf_readwrite;
     unsigned long storageOf_delim;
     unsigned long storageOf_pad;
     unsigned long storageOf_asynchronous;
     unsigned long storageOf_decimal;
     unsigned long storageOf_stream;
     unsigned long storageOf_size;
     unsigned long storageOf_pending;
     unsigned long storageOf_iolengthExp;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInquireStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInquireStatement;
   };
/* #line 15905 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFlushStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFlushStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15920 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFlushStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFlushStatement;
   };
/* #line 15939 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBackspaceStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBackspaceStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15954 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBackspaceStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBackspaceStatement;
   };
/* #line 15973 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRewindStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRewindStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 15988 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRewindStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRewindStatement;
   };
/* #line 16007 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEndfileStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEndfileStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 16022 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEndfileStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEndfileStatement;
   };
/* #line 16041 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWaitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWaitStatementStorageClass  : public SgIOStatementStorageClass
   {

    protected: 


/* #line 16056 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWaitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWaitStatement;
   };
/* #line 16075 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWhereStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWhereStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16090 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
     unsigned long storageOf_body;
     unsigned long storageOf_elsewhere;
     unsigned long storageOf_end_numeric_label;
       EasyStorage < std::string > storageOf_string_label;
      bool storageOf_has_end_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWhereStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWhereStatement;
   };
/* #line 16115 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElseWhereStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElseWhereStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16130 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_condition;
     unsigned long storageOf_body;
     unsigned long storageOf_elsewhere;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElseWhereStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElseWhereStatement;
   };
/* #line 16152 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNullifyStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNullifyStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16167 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_pointer_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNullifyStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNullifyStatement;
   };
/* #line 16187 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgArithmeticIfStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgArithmeticIfStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16202 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_conditional;
     unsigned long storageOf_less_label;
     unsigned long storageOf_equal_label;
     unsigned long storageOf_greater_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgArithmeticIfStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgArithmeticIfStatement;
   };
/* #line 16225 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssignStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssignStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16240 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_label;
     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssignStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssignStatement;
   };
/* #line 16261 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgComputedGotoStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgComputedGotoStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16276 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_labelList;
     unsigned long storageOf_label_index;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgComputedGotoStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgComputedGotoStatement;
   };
/* #line 16297 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssignedGotoStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssignedGotoStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16312 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_targets;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssignedGotoStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssignedGotoStatement;
   };
/* #line 16332 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAllocateStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAllocateStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16347 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expr_list;
     unsigned long storageOf_stat_expression;
     unsigned long storageOf_errmsg_expression;
     unsigned long storageOf_source_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAllocateStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAllocateStatement;
   };
/* #line 16370 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeallocateStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeallocateStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16385 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expr_list;
     unsigned long storageOf_stat_expression;
     unsigned long storageOf_errmsg_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeallocateStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeallocateStatement;
   };
/* #line 16407 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcNotifyStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcNotifyStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16422 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_notify_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcNotifyStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcNotifyStatement;
   };
/* #line 16442 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcWaitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcWaitStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16457 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_wait_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcWaitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcWaitStatement;
   };
/* #line 16477 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcBarrierStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcBarrierStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16492 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_barrier_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcBarrierStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcBarrierStatement;
   };
/* #line 16512 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcFenceStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcFenceStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16527 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcFenceStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcFenceStatement;
   };
/* #line 16546 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpBarrierStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpBarrierStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16561 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpBarrierStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpBarrierStatement;
   };
/* #line 16580 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpTaskwaitStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpTaskwaitStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16595 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpTaskwaitStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpTaskwaitStatement;
   };
/* #line 16614 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpFlushStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpFlushStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16629 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgVarRefExpPtrList > storageOf_variables;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpFlushStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpFlushStatement;
   };
/* #line 16649 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpBodyStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpBodyStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 16664 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpBodyStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpBodyStatement;
   };
/* #line 16684 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpMasterStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpMasterStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16699 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpMasterStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpMasterStatement;
   };
/* #line 16718 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpOrderedStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpOrderedStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16733 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpOrderedStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpOrderedStatement;
   };
/* #line 16752 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpCriticalStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpCriticalStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16767 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpCriticalStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpCriticalStatement;
   };
/* #line 16787 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSectionStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSectionStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16802 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSectionStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSectionStatement;
   };
/* #line 16821 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpWorkshareStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpWorkshareStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16836 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpWorkshareStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpWorkshareStatement;
   };
/* #line 16855 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpClauseBodyStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpClauseBodyStatementStorageClass  : public SgOmpBodyStatementStorageClass
   {

    protected: 


/* #line 16870 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgOmpClausePtrList > storageOf_clauses;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpClauseBodyStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpClauseBodyStatement;
   };
/* #line 16890 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpParallelStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpParallelStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 16905 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpParallelStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpParallelStatement;
   };
/* #line 16924 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSingleStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSingleStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 16939 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSingleStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSingleStatement;
   };
/* #line 16958 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpAtomicStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpAtomicStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 16973 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpAtomicStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpAtomicStatement;
   };
/* #line 16992 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpTaskStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpTaskStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17007 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpTaskStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpTaskStatement;
   };
/* #line 17026 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpForStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpForStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17041 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpForStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpForStatement;
   };
/* #line 17060 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpDoStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpDoStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17075 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpDoStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpDoStatement;
   };
/* #line 17094 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSectionsStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSectionsStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17109 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSectionsStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSectionsStatement;
   };
/* #line 17128 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpTargetStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpTargetStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17143 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpTargetStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpTargetStatement;
   };
/* #line 17162 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpTargetDataStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpTargetDataStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17177 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpTargetDataStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpTargetDataStatement;
   };
/* #line 17196 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpSimdStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpSimdStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17211 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpSimdStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpSimdStatement;
   };
/* #line 17230 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOmpForSimdStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOmpForSimdStatementStorageClass  : public SgOmpClauseBodyStatementStorageClass
   {

    protected: 


/* #line 17245 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOmpForSimdStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOmpForSimdStatement;
   };
/* #line 17264 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSequenceStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSequenceStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17279 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSequenceStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSequenceStatement;
   };
/* #line 17298 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWithStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWithStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17313 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWithStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWithStatement;
   };
/* #line 17334 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPythonPrintStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPythonPrintStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17349 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_destination;
     unsigned long storageOf_values;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPythonPrintStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPythonPrintStmt;
   };
/* #line 17370 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPassStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPassStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17385 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPassStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPassStatement;
   };
/* #line 17404 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssertStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssertStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17419 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_test;
     unsigned long storageOf_exception_argument;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssertStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssertStmt;
   };
/* #line 17440 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExecStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExecStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17455 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_executable;
     unsigned long storageOf_globals;
     unsigned long storageOf_locals;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExecStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExecStatement;
   };
/* #line 17477 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPythonGlobalStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPythonGlobalStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17492 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgInitializedNamePtrList > storageOf_names;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPythonGlobalStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPythonGlobalStmt;
   };
/* #line 17512 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaThrowStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaThrowStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17527 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_throwOp;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaThrowStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaThrowStatement;
   };
/* #line 17547 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaSynchronizedStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaSynchronizedStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17562 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaSynchronizedStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaSynchronizedStatement;
   };
/* #line 17583 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAsyncStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAsyncStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17598 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAsyncStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAsyncStmt;
   };
/* #line 17618 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFinishStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFinishStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17633 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFinishStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFinishStmt;
   };
/* #line 17653 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAtStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAtStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17668 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAtStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAtStmt;
   };
/* #line 17689 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAtomicStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAtomicStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17704 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAtomicStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAtomicStmt;
   };
/* #line 17724 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWhenStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWhenStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17739 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWhenStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWhenStmt;
   };
/* #line 17760 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgImageControlStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgImageControlStatementStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 17775 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgImageControlStatement::image_control_statement_enum storageOf_image_control_statement;
     unsigned long storageOf_stat;
     unsigned long storageOf_err_msg;
     unsigned long storageOf_acquired_lock;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgImageControlStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgImageControlStatement;
   };
/* #line 17798 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSyncAllStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSyncAllStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17813 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSyncAllStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSyncAllStatement;
   };
/* #line 17832 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSyncImagesStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSyncImagesStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17847 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_image_set;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSyncImagesStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSyncImagesStatement;
   };
/* #line 17867 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSyncMemoryStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSyncMemoryStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17882 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSyncMemoryStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSyncMemoryStatement;
   };
/* #line 17901 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSyncTeamStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSyncTeamStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17916 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_team_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSyncTeamStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSyncTeamStatement;
   };
/* #line 17936 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLockStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLockStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17951 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lock_variable;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLockStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLockStatement;
   };
/* #line 17971 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnlockStatementStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnlockStatementStorageClass  : public SgImageControlStatementStorageClass
   {

    protected: 


/* #line 17986 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lock_variable;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnlockStatement* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnlockStatement;
   };
/* #line 18006 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaExitStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaExitStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 18021 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_loop;
     unsigned long storageOf_condition;
      bool storageOf_explicitLoopName;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaExitStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaExitStmt;
   };
/* #line 18043 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaLoopStmtStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaLoopStmtStorageClass  : public SgStatementStorageClass
   {

    protected: 


/* #line 18058 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_body;
       EasyStorage < std::string > storageOf_string_label;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaLoopStmt* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaLoopStmt;
   };
/* #line 18079 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExpressionStorageClass  : public SgLocatedNodeStorageClass
   {

    protected: 


/* #line 18094 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_need_paren;
      bool storageOf_lvalue;
      bool storageOf_global_qualified_name;
     unsigned long storageOf_operatorPosition;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExpression;
   };
/* #line 18117 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnaryOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnaryOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 18132 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_i;
     unsigned long storageOf_expression_type;
      SgUnaryOp::Sgop_mode storageOf_mode;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnaryOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnaryOp;
   };
/* #line 18154 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExpressionRootStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExpressionRootStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18169 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExpressionRoot* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExpressionRoot;
   };
/* #line 18188 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMinusOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMinusOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18203 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMinusOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMinusOp;
   };
/* #line 18222 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnaryAddOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnaryAddOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18237 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnaryAddOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnaryAddOp;
   };
/* #line 18256 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNotOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNotOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18271 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNotOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNotOp;
   };
/* #line 18290 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPointerDerefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPointerDerefExpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18305 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPointerDerefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPointerDerefExp;
   };
/* #line 18324 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAddressOfOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAddressOfOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18339 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_originalExpressionTree;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAddressOfOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAddressOfOp;
   };
/* #line 18359 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMinusMinusOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMinusMinusOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18374 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMinusMinusOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMinusMinusOp;
   };
/* #line 18393 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPlusPlusOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPlusPlusOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18408 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPlusPlusOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPlusPlusOp;
   };
/* #line 18427 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitComplementOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitComplementOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18442 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitComplementOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitComplementOp;
   };
/* #line 18461 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCastExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCastExpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18476 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgCastExp::cast_type_enum storageOf_cast_type;
     unsigned long storageOf_originalExpressionTree;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_castContainsBaseTypeDefiningDeclaration;
      int storageOf_name_qualification_for_pointer_to_member_class_length;
      bool storageOf_type_elaboration_for_pointer_to_member_class_required;
      bool storageOf_global_qualification_for_pointer_to_member_class_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCastExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCastExp;
   };
/* #line 18504 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgThrowOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgThrowOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18519 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgThrowOp::e_throw_kind storageOf_throwKind;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgThrowOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgThrowOp;
   };
/* #line 18539 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRealPartOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRealPartOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18554 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRealPartOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRealPartOp;
   };
/* #line 18573 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgImagPartOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgImagPartOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18588 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgImagPartOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgImagPartOp;
   };
/* #line 18607 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgConjugateOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgConjugateOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18622 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgConjugateOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgConjugateOp;
   };
/* #line 18641 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUserDefinedUnaryOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUserDefinedUnaryOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18656 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_operator_name;
     unsigned long storageOf_symbol;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUserDefinedUnaryOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUserDefinedUnaryOp;
   };
/* #line 18677 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMatrixTransposeOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMatrixTransposeOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18692 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_is_conjugate;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMatrixTransposeOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMatrixTransposeOp;
   };
/* #line 18712 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAbsOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAbsOpStorageClass  : public SgUnaryOpStorageClass
   {

    protected: 


/* #line 18727 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAbsOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAbsOp;
   };
/* #line 18746 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBinaryOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBinaryOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 18761 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lhs_operand_i;
     unsigned long storageOf_rhs_operand_i;
     unsigned long storageOf_expression_type;
     unsigned long storageOf_originalExpressionTree;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBinaryOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBinaryOp;
   };
/* #line 18784 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgArrowExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgArrowExpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18799 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgArrowExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgArrowExp;
   };
/* #line 18818 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDotExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDotExpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18833 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDotExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDotExp;
   };
/* #line 18852 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDotStarOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDotStarOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18867 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDotStarOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDotStarOp;
   };
/* #line 18886 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgArrowStarOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgArrowStarOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18901 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgArrowStarOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgArrowStarOp;
   };
/* #line 18920 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEqualityOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEqualityOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18935 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEqualityOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEqualityOp;
   };
/* #line 18954 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLessThanOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLessThanOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 18969 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLessThanOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLessThanOp;
   };
/* #line 18988 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGreaterThanOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGreaterThanOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19003 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGreaterThanOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGreaterThanOp;
   };
/* #line 19022 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNotEqualOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNotEqualOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19037 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNotEqualOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNotEqualOp;
   };
/* #line 19056 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLessOrEqualOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLessOrEqualOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19071 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLessOrEqualOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLessOrEqualOp;
   };
/* #line 19090 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgGreaterOrEqualOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgGreaterOrEqualOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19105 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgGreaterOrEqualOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgGreaterOrEqualOp;
   };
/* #line 19124 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAddOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAddOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19139 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAddOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAddOp;
   };
/* #line 19158 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSubtractOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSubtractOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19173 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSubtractOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSubtractOp;
   };
/* #line 19192 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMultiplyOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMultiplyOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19207 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMultiplyOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMultiplyOp;
   };
/* #line 19226 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDivideOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDivideOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19241 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDivideOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDivideOp;
   };
/* #line 19260 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIntegerDivideOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIntegerDivideOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19275 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIntegerDivideOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIntegerDivideOp;
   };
/* #line 19294 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19309 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModOp;
   };
/* #line 19328 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAndOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAndOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19343 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAndOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAndOp;
   };
/* #line 19362 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgOrOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgOrOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19377 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgOrOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgOrOp;
   };
/* #line 19396 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitXorOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitXorOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19411 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitXorOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitXorOp;
   };
/* #line 19430 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitAndOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitAndOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19445 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitAndOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitAndOp;
   };
/* #line 19464 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitOrOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitOrOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19479 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitOrOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitOrOp;
   };
/* #line 19498 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBitEqvOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBitEqvOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19513 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBitEqvOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBitEqvOp;
   };
/* #line 19532 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCommaOpExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCommaOpExpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19547 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCommaOpExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCommaOpExp;
   };
/* #line 19566 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLshiftOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLshiftOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19581 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLshiftOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLshiftOp;
   };
/* #line 19600 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRshiftOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRshiftOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19615 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRshiftOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRshiftOp;
   };
/* #line 19634 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPntrArrRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPntrArrRefExpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19649 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPntrArrRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPntrArrRefExp;
   };
/* #line 19668 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgScopeOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgScopeOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19683 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgScopeOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgScopeOp;
   };
/* #line 19702 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssignOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19717 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssignOp;
   };
/* #line 19736 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExponentiationOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExponentiationOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19751 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExponentiationOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExponentiationOp;
   };
/* #line 19770 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaUnsignedRshiftOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaUnsignedRshiftOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19785 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaUnsignedRshiftOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaUnsignedRshiftOp;
   };
/* #line 19804 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgConcatenationOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgConcatenationOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19819 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgConcatenationOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgConcatenationOp;
   };
/* #line 19838 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPointerAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPointerAssignOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19853 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPointerAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPointerAssignOp;
   };
/* #line 19872 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUserDefinedBinaryOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUserDefinedBinaryOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19887 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_operator_name;
     unsigned long storageOf_symbol;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUserDefinedBinaryOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUserDefinedBinaryOp;
   };
/* #line 19908 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCompoundAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCompoundAssignOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 19923 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCompoundAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCompoundAssignOp;
   };
/* #line 19942 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPlusAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPlusAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 19957 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPlusAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPlusAssignOp;
   };
/* #line 19976 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMinusAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMinusAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 19991 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMinusAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMinusAssignOp;
   };
/* #line 20010 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAndAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAndAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20025 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAndAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAndAssignOp;
   };
/* #line 20044 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIorAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIorAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20059 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIorAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIorAssignOp;
   };
/* #line 20078 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMultAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMultAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20093 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMultAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMultAssignOp;
   };
/* #line 20112 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDivAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDivAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20127 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDivAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDivAssignOp;
   };
/* #line 20146 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20161 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModAssignOp;
   };
/* #line 20180 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgXorAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgXorAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20195 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgXorAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgXorAssignOp;
   };
/* #line 20214 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLshiftAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLshiftAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20229 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLshiftAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLshiftAssignOp;
   };
/* #line 20248 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRshiftAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRshiftAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20263 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRshiftAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRshiftAssignOp;
   };
/* #line 20282 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaUnsignedRshiftAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaUnsignedRshiftAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20297 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaUnsignedRshiftAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaUnsignedRshiftAssignOp;
   };
/* #line 20316 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIntegerDivideAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIntegerDivideAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20331 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIntegerDivideAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIntegerDivideAssignOp;
   };
/* #line 20350 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExponentiationAssignOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExponentiationAssignOpStorageClass  : public SgCompoundAssignOpStorageClass
   {

    protected: 


/* #line 20365 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExponentiationAssignOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExponentiationAssignOp;
   };
/* #line 20384 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMembershipOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMembershipOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20399 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMembershipOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMembershipOp;
   };
/* #line 20418 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSpaceshipOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSpaceshipOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20433 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSpaceshipOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSpaceshipOp;
   };
/* #line 20452 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonMembershipOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonMembershipOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20467 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonMembershipOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonMembershipOp;
   };
/* #line 20486 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIsOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIsOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20501 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIsOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIsOp;
   };
/* #line 20520 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIsNotOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIsNotOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20535 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIsNotOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIsNotOp;
   };
/* #line 20554 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDotDotExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDotDotExpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20569 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDotDotExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDotDotExp;
   };
/* #line 20588 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20603 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseOp;
   };
/* #line 20622 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseMultiplyOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseMultiplyOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20637 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseMultiplyOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseMultiplyOp;
   };
/* #line 20656 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwisePowerOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwisePowerOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20671 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwisePowerOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwisePowerOp;
   };
/* #line 20690 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseLeftDivideOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseLeftDivideOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20705 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseLeftDivideOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseLeftDivideOp;
   };
/* #line 20724 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseDivideOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseDivideOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20739 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseDivideOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseDivideOp;
   };
/* #line 20758 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseAddOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseAddOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20773 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseAddOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseAddOp;
   };
/* #line 20792 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgElementwiseSubtractOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgElementwiseSubtractOpStorageClass  : public SgElementwiseOpStorageClass
   {

    protected: 


/* #line 20807 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgElementwiseSubtractOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgElementwiseSubtractOp;
   };
/* #line 20826 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPowerOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPowerOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20841 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPowerOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPowerOp;
   };
/* #line 20860 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLeftDivideOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLeftDivideOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20875 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLeftDivideOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLeftDivideOp;
   };
/* #line 20894 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRemOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRemOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20909 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRemOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRemOp;
   };
/* #line 20928 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgReplicationOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgReplicationOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20943 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgReplicationOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgReplicationOp;
   };
/* #line 20962 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAtOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAtOpStorageClass  : public SgBinaryOpStorageClass
   {

    protected: 


/* #line 20977 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAtOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAtOp;
   };
/* #line 20996 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgExprListExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgExprListExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21011 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgExpressionPtrList > storageOf_expressions;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgExprListExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgExprListExp;
   };
/* #line 21031 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgListExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgListExpStorageClass  : public SgExprListExpStorageClass
   {

    protected: 


/* #line 21046 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgListExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgListExp;
   };
/* #line 21065 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTupleExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTupleExpStorageClass  : public SgExprListExpStorageClass
   {

    protected: 


/* #line 21080 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTupleExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTupleExp;
   };
/* #line 21099 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMatrixExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMatrixExpStorageClass  : public SgExprListExpStorageClass
   {

    protected: 


/* #line 21114 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMatrixExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMatrixExp;
   };
/* #line 21133 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21148 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol;
     unsigned long storageOf_originalExpressionTree;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarRefExp;
   };
/* #line 21172 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassNameRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassNameRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21187 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassNameRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassNameRefExp;
   };
/* #line 21207 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21222 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol_i;
     unsigned long storageOf_function_type;
     unsigned long storageOf_originalExpressionTree;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionRefExp;
   };
/* #line 21247 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMemberFunctionRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMemberFunctionRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21262 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol_i;
      int storageOf_virtual_call;
     unsigned long storageOf_function_type;
      int storageOf_need_qualifier;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMemberFunctionRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMemberFunctionRefExp;
   };
/* #line 21288 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgValueExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgValueExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 21303 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_originalExpressionTree;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgValueExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgValueExp;
   };
/* #line 21323 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBoolValExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBoolValExpStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21338 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBoolValExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBoolValExp;
   };
/* #line 21358 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStringValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStringValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21373 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_value;
      bool storageOf_wcharString;
      bool storageOf_usesSingleQuotes;
      bool storageOf_usesDoubleQuotes;
      bool storageOf_is16bitString;
      bool storageOf_is32bitString;
      bool storageOf_isRawString;
       EasyStorage < std::string > storageOf_raw_string_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStringVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStringVal;
   };
/* #line 21400 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgShortValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgShortValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21415 "../../../src/frontend/SageIII//StorageClasses.h" */

      short storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgShortVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgShortVal;
   };
/* #line 21436 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCharValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCharValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21451 "../../../src/frontend/SageIII//StorageClasses.h" */

      char storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCharVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCharVal;
   };
/* #line 21472 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnsignedCharValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnsignedCharValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21487 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned char storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnsignedCharVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnsignedCharVal;
   };
/* #line 21508 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgWcharValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgWcharValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21523 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned long storageOf_valueUL;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgWcharVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgWcharVal;
   };
/* #line 21544 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnsignedShortValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnsignedShortValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21559 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned short storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnsignedShortVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnsignedShortVal;
   };
/* #line 21580 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIntValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIntValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21595 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIntVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIntVal;
   };
/* #line 21616 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEnumValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEnumValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21631 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_value;
     unsigned long storageOf_declaration;
      SgNameStorageClass storageOf_name;
      bool storageOf_requiresNameQualification;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEnumVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEnumVal;
   };
/* #line 21657 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnsignedIntValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnsignedIntValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21672 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnsignedIntVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnsignedIntVal;
   };
/* #line 21693 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLongIntValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLongIntValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21708 "../../../src/frontend/SageIII//StorageClasses.h" */

      long int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLongIntVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLongIntVal;
   };
/* #line 21729 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLongLongIntValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLongLongIntValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21744 "../../../src/frontend/SageIII//StorageClasses.h" */

      long long int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLongLongIntVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLongLongIntVal;
   };
/* #line 21765 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnsignedLongLongIntValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnsignedLongLongIntValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21780 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned long long int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnsignedLongLongIntVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnsignedLongLongIntVal;
   };
/* #line 21801 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnsignedLongValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnsignedLongValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21816 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned long storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnsignedLongVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnsignedLongVal;
   };
/* #line 21837 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFloatValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFloatValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21852 "../../../src/frontend/SageIII//StorageClasses.h" */

      float storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFloatVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFloatVal;
   };
/* #line 21873 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDoubleValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDoubleValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21888 "../../../src/frontend/SageIII//StorageClasses.h" */

      double storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDoubleVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDoubleVal;
   };
/* #line 21909 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLongDoubleValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLongDoubleValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21924 "../../../src/frontend/SageIII//StorageClasses.h" */

      long double storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLongDoubleVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLongDoubleVal;
   };
/* #line 21945 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgComplexValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgComplexValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21960 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_real_value;
     unsigned long storageOf_imaginary_value;
     unsigned long storageOf_precisionType;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgComplexVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgComplexVal;
   };
/* #line 21983 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcThreadsStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcThreadsStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 21998 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcThreads* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcThreads;
   };
/* #line 22019 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcMythreadStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcMythreadStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22034 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcMythread* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcMythread;
   };
/* #line 22055 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateParameterValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateParameterValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22070 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_template_parameter_position;
       EasyStorage < std::string > storageOf_valueString;
     unsigned long storageOf_valueType;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateParameterVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateParameterVal;
   };
/* #line 22092 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNullptrValExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNullptrValExpStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22107 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNullptrValExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNullptrValExp;
   };
/* #line 22126 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgChar16ValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgChar16ValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22141 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned short storageOf_valueUL;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgChar16Val* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgChar16Val;
   };
/* #line 22162 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgChar32ValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgChar32ValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22177 "../../../src/frontend/SageIII//StorageClasses.h" */

      unsigned int storageOf_valueUL;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgChar32Val* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgChar32Val;
   };
/* #line 22198 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFloat80ValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFloat80ValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22213 "../../../src/frontend/SageIII//StorageClasses.h" */

      long double storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFloat80Val* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFloat80Val;
   };
/* #line 22234 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFloat128ValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFloat128ValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22249 "../../../src/frontend/SageIII//StorageClasses.h" */

      long double storageOf_value;
       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFloat128Val* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFloat128Val;
   };
/* #line 22270 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVoidValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVoidValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22285 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVoidVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVoidVal;
   };
/* #line 22304 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaFloatValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaFloatValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22319 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaFloatVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaFloatVal;
   };
/* #line 22339 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialBitValStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialBitValStorageClass  : public SgValueExpStorageClass
   {

    protected: 


/* #line 22354 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_valueString;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialBitVal* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialBitVal;
   };
/* #line 22374 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCallExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCallExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22389 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_function;
     unsigned long storageOf_args;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCallExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCallExpression;
   };
/* #line 22411 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionCallExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionCallExpStorageClass  : public SgCallExpressionStorageClass
   {

    protected: 


/* #line 22426 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_uses_operator_syntax;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionCallExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionCallExp;
   };
/* #line 22446 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCudaKernelCallExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCudaKernelCallExpStorageClass  : public SgFunctionCallExpStorageClass
   {

    protected: 


/* #line 22461 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_exec_config;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCudaKernelCallExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCudaKernelCallExp;
   };
/* #line 22481 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSizeOfOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSizeOfOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22496 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_sizeOfContainsBaseTypeDefiningDeclaration;
      bool storageOf_is_objectless_nonstatic_data_member_reference;
      int storageOf_name_qualification_for_pointer_to_member_class_length;
      bool storageOf_type_elaboration_for_pointer_to_member_class_required;
      bool storageOf_global_qualification_for_pointer_to_member_class_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSizeOfOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSizeOfOp;
   };
/* #line 22526 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcLocalsizeofExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcLocalsizeofExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22541 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcLocalsizeofExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcLocalsizeofExpression;
   };
/* #line 22563 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcBlocksizeofExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcBlocksizeofExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22578 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcBlocksizeofExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcBlocksizeofExpression;
   };
/* #line 22600 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUpcElemsizeofExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUpcElemsizeofExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22615 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUpcElemsizeofExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUpcElemsizeofExpression;
   };
/* #line 22637 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaInstanceOfOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaInstanceOfOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22652 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaInstanceOfOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaInstanceOfOp;
   };
/* #line 22674 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSuperExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSuperExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22689 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_class_symbol;
      int storageOf_pobj_super;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSuperExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSuperExp;
   };
/* #line 22710 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeIdOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeIdOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22725 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_operand_type;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      int storageOf_name_qualification_for_pointer_to_member_class_length;
      bool storageOf_type_elaboration_for_pointer_to_member_class_required;
      bool storageOf_global_qualification_for_pointer_to_member_class_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeIdOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeIdOp;
   };
/* #line 22752 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgConditionalExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgConditionalExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22767 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_conditional_exp;
     unsigned long storageOf_true_exp;
     unsigned long storageOf_false_exp;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgConditionalExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgConditionalExp;
   };
/* #line 22790 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNewExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNewExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22805 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_specified_type;
     unsigned long storageOf_placement_args;
     unsigned long storageOf_constructor_args;
     unsigned long storageOf_builtin_args;
      short storageOf_need_global_specifier;
     unsigned long storageOf_newOperatorDeclaration;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      int storageOf_name_qualification_for_pointer_to_member_class_length;
      bool storageOf_type_elaboration_for_pointer_to_member_class_required;
      bool storageOf_global_qualification_for_pointer_to_member_class_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNewExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNewExp;
   };
/* #line 22836 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDeleteExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDeleteExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22851 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_variable;
      short storageOf_is_array;
      short storageOf_need_global_specifier;
     unsigned long storageOf_deleteOperatorDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDeleteExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDeleteExp;
   };
/* #line 22874 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgThisExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgThisExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22889 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_class_symbol;
     unsigned long storageOf_nonreal_symbol;
      int storageOf_pobj_this;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgThisExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgThisExp;
   };
/* #line 22911 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22926 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRefExp;
   };
/* #line 22946 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInitializerStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 22961 "../../../src/frontend/SageIII//StorageClasses.h" */

      bool storageOf_is_explicit_cast;
      bool storageOf_is_braced_initialized;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInitializer;
   };
/* #line 22982 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAggregateInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAggregateInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 22997 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_initializers;
     unsigned long storageOf_expression_type;
      bool storageOf_need_explicit_braces;
      bool storageOf_uses_compound_literal;
      bool storageOf_requiresGlobalNameQualificationOnType;
      int storageOf_name_qualification_length_for_type;
      bool storageOf_type_elaboration_required_for_type;
      bool storageOf_global_qualification_required_for_type;
     unsigned long storageOf_originalExpressionTree;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAggregateInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAggregateInitializer;
   };
/* #line 23025 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCompoundInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCompoundInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 23040 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_initializers;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCompoundInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCompoundInitializer;
   };
/* #line 23061 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgConstructorInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgConstructorInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 23076 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;
     unsigned long storageOf_args;
     unsigned long storageOf_expression_type;
      bool storageOf_need_name;
      bool storageOf_need_qualifier;
      bool storageOf_need_parenthesis_after_name;
      bool storageOf_associated_class_unknown;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_is_used_in_conditional;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgConstructorInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgConstructorInitializer;
   };
/* #line 23106 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAssignInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAssignInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 23121 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_i;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAssignInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAssignInitializer;
   };
/* #line 23142 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDesignatedInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDesignatedInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 23157 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_designatorList;
     unsigned long storageOf_memberInit;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDesignatedInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDesignatedInitializer;
   };
/* #line 23178 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgBracedInitializerStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgBracedInitializerStorageClass  : public SgInitializerStorageClass
   {

    protected: 


/* #line 23193 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_initializers;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgBracedInitializer* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgBracedInitializer;
   };
/* #line 23214 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarArgStartOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarArgStartOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23229 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lhs_operand;
     unsigned long storageOf_rhs_operand;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarArgStartOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarArgStartOp;
   };
/* #line 23251 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarArgOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarArgOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23266 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarArgOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarArgOp;
   };
/* #line 23287 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarArgEndOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarArgEndOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23302 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarArgEndOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarArgEndOp;
   };
/* #line 23323 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarArgCopyOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarArgCopyOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23338 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lhs_operand;
     unsigned long storageOf_rhs_operand;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarArgCopyOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarArgCopyOp;
   };
/* #line 23360 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVarArgStartOneOperandOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVarArgStartOneOperandOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23375 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVarArgStartOneOperandOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVarArgStartOneOperandOp;
   };
/* #line 23396 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNullExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNullExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23411 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNullExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNullExpression;
   };
/* #line 23430 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVariantExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVariantExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23445 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVariantExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVariantExpression;
   };
/* #line 23464 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSubscriptExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSubscriptExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23479 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lowerBound;
     unsigned long storageOf_upperBound;
     unsigned long storageOf_stride;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSubscriptExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSubscriptExpression;
   };
/* #line 23501 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgColonShapeExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgColonShapeExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23516 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgColonShapeExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgColonShapeExp;
   };
/* #line 23535 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAsteriskShapeExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAsteriskShapeExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23550 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAsteriskShapeExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAsteriskShapeExp;
   };
/* #line 23569 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgImpliedDoStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgImpliedDoStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23584 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_do_var_initialization;
     unsigned long storageOf_last_val;
     unsigned long storageOf_increment;
     unsigned long storageOf_object_list;
     unsigned long storageOf_implied_do_scope;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgImpliedDo* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgImpliedDo;
   };
/* #line 23608 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIOItemExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIOItemExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23623 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_io_item;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIOItemExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIOItemExpression;
   };
/* #line 23644 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStatementExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStatementExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23659 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_statement;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStatementExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStatementExpression;
   };
/* #line 23679 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAsmOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAsmOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23694 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgAsmOp::asm_operand_constraint_enum storageOf_constraint;
      SgAsmOp::asm_operand_modifier_enum storageOf_modifiers;
     unsigned long storageOf_expression;
      bool storageOf_recordRawAsmOperandDescriptions;
      bool storageOf_isOutputOperand;
       EasyStorage < std::string > storageOf_constraintString;
       EasyStorage < std::string > storageOf_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAsmOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAsmOp;
   };
/* #line 23720 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLabelRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLabelRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23735 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLabelRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLabelRefExp;
   };
/* #line 23755 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgActualArgumentExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgActualArgumentExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23770 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_argument_name;
     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgActualArgumentExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgActualArgumentExpression;
   };
/* #line 23791 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgUnknownArrayOrFunctionReferenceStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgUnknownArrayOrFunctionReferenceStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23806 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < std::string > storageOf_name;
     unsigned long storageOf_named_reference;
     unsigned long storageOf_expression_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgUnknownArrayOrFunctionReference* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgUnknownArrayOrFunctionReference;
   };
/* #line 23828 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgPseudoDestructorRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgPseudoDestructorRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23843 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_object_type;
     unsigned long storageOf_expression_type;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgPseudoDestructorRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgPseudoDestructorRefExp;
   };
/* #line 23867 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCAFCoExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCAFCoExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23882 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_teamId;
     unsigned long storageOf_teamRank;
     unsigned long storageOf_referData;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCAFCoExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCAFCoExpression;
   };
/* #line 23904 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCudaKernelExecConfigStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCudaKernelExecConfigStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23919 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_grid;
     unsigned long storageOf_blocks;
     unsigned long storageOf_shared;
     unsigned long storageOf_stream;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCudaKernelExecConfig* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCudaKernelExecConfig;
   };
/* #line 23942 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLambdaRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLambdaRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23957 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_functionDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLambdaRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLambdaRefExp;
   };
/* #line 23977 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDictionaryExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDictionaryExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 23992 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgKeyDatumPairPtrList > storageOf_key_datum_pairs;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDictionaryExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDictionaryExp;
   };
/* #line 24012 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgKeyDatumPairStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgKeyDatumPairStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24027 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_key;
     unsigned long storageOf_datum;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgKeyDatumPair* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgKeyDatumPair;
   };
/* #line 24048 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgComprehensionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgComprehensionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24063 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_target;
     unsigned long storageOf_iter;
     unsigned long storageOf_filters;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgComprehension* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgComprehension;
   };
/* #line 24085 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgListComprehensionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgListComprehensionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24100 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_element;
     unsigned long storageOf_generators;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgListComprehension* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgListComprehension;
   };
/* #line 24121 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSetComprehensionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSetComprehensionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24136 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_element;
     unsigned long storageOf_generators;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSetComprehension* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSetComprehension;
   };
/* #line 24157 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDictionaryComprehensionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDictionaryComprehensionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24172 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_element;
     unsigned long storageOf_generators;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDictionaryComprehension* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDictionaryComprehension;
   };
/* #line 24193 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNaryOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNaryOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24208 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgExpressionPtrList > storageOf_operands;
       EasyStorage < VariantTList > storageOf_operators;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNaryOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNaryOp;
   };
/* #line 24229 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNaryBooleanOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNaryBooleanOpStorageClass  : public SgNaryOpStorageClass
   {

    protected: 


/* #line 24244 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNaryBooleanOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNaryBooleanOp;
   };
/* #line 24263 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNaryComparisonOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNaryComparisonOpStorageClass  : public SgNaryOpStorageClass
   {

    protected: 


/* #line 24278 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNaryComparisonOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNaryComparisonOp;
   };
/* #line 24297 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgStringConversionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgStringConversionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24312 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgStringConversion* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgStringConversion;
   };
/* #line 24332 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgYieldExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgYieldExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24347 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgYieldExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgYieldExpression;
   };
/* #line 24367 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateFunctionRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateFunctionRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24382 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol_i;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateFunctionRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateFunctionRefExp;
   };
/* #line 24405 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateMemberFunctionRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateMemberFunctionRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24420 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol_i;
      int storageOf_virtual_call;
      int storageOf_need_qualifier;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateMemberFunctionRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateMemberFunctionRefExp;
   };
/* #line 24445 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAlignOfOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAlignOfOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24460 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;
     unsigned long storageOf_operand_type;
     unsigned long storageOf_expression_type;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;
      bool storageOf_alignOfContainsBaseTypeDefiningDeclaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAlignOfOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAlignOfOp;
   };
/* #line 24486 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRangeExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRangeExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24501 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_start;
     unsigned long storageOf_end;
     unsigned long storageOf_stride;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRangeExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRangeExp;
   };
/* #line 24523 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMagicColonExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMagicColonExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24538 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMagicColonExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMagicColonExp;
   };
/* #line 24557 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeTraitBuiltinOperatorStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeTraitBuiltinOperatorStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24572 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
       EasyStorage < SgNodePtrList > storageOf_args;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeTraitBuiltinOperator* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeTraitBuiltinOperator;
   };
/* #line 24593 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCompoundLiteralExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCompoundLiteralExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24608 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCompoundLiteralExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCompoundLiteralExp;
   };
/* #line 24628 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaAnnotationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaAnnotationStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24643 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaAnnotation* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaAnnotation;
   };
/* #line 24663 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaMarkerAnnotationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaMarkerAnnotationStorageClass  : public SgJavaAnnotationStorageClass
   {

    protected: 


/* #line 24678 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaMarkerAnnotation* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaMarkerAnnotation;
   };
/* #line 24697 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaSingleMemberAnnotationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaSingleMemberAnnotationStorageClass  : public SgJavaAnnotationStorageClass
   {

    protected: 


/* #line 24712 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaSingleMemberAnnotation* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaSingleMemberAnnotation;
   };
/* #line 24732 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaNormalAnnotationStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaNormalAnnotationStorageClass  : public SgJavaAnnotationStorageClass
   {

    protected: 


/* #line 24747 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < SgJavaMemberValuePairPtrList > storageOf_value_pair_list;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaNormalAnnotation* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaNormalAnnotation;
   };
/* #line 24767 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaTypeExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaTypeExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24782 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaTypeExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaTypeExpression;
   };
/* #line 24802 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypeExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypeExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24817 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypeExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypeExpression;
   };
/* #line 24837 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24852 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_class_symbol;
      int storageOf_pobj_class;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassExp;
   };
/* #line 24873 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionParameterRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionParameterRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24888 "../../../src/frontend/SageIII//StorageClasses.h" */

      int storageOf_parameter_number;
      int storageOf_parameter_levels_up;
     unsigned long storageOf_parameter_expression;
     unsigned long storageOf_parameter_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionParameterRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionParameterRefExp;
   };
/* #line 24911 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLambdaExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLambdaExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24926 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_lambda_capture_list;
     unsigned long storageOf_lambda_closure_class;
     unsigned long storageOf_lambda_function;
      bool storageOf_is_mutable;
      bool storageOf_capture_default;
      bool storageOf_default_is_by_reference;
      bool storageOf_explicit_return_type;
      bool storageOf_has_parameter_decl;
      bool storageOf_is_device;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLambdaExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLambdaExp;
   };
/* #line 24954 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgHereExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgHereExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 24969 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression_type;
     unsigned long storageOf_expression;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgHereExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgHereExp;
   };
/* #line 24990 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAtExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAtExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25005 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAtExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAtExp;
   };
/* #line 25026 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFinishExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFinishExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25041 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_expression;
     unsigned long storageOf_body;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFinishExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFinishExp;
   };
/* #line 25062 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNoexceptOpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNoexceptOpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25077 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operand_expr;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNoexceptOp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNoexceptOp;
   };
/* #line 25097 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonrealRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonrealRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25112 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_symbol;
      int storageOf_name_qualification_length;
      bool storageOf_type_elaboration_required;
      bool storageOf_global_qualification_required;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonrealRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonrealRefExp;
   };
/* #line 25135 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskRefExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskRefExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25150 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_decl;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskRefExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskRefExp;
   };
/* #line 25170 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFoldExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFoldExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25185 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_operands;
       EasyStorage < std::string > storageOf_operator_token;
      bool storageOf_is_left_associative;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFoldExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFoldExpression;
   };
/* #line 25207 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAwaitExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAwaitExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25222 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAwaitExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAwaitExpression;
   };
/* #line 25242 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgChooseExpressionStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgChooseExpressionStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25257 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_value;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgChooseExpression* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgChooseExpression;
   };
/* #line 25277 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJovialTablePresetExpStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJovialTablePresetExpStorageClass  : public SgExpressionStorageClass
   {

    protected: 


/* #line 25292 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_default_sublist;
     unsigned long storageOf_specified_sublist;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJovialTablePresetExp* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJovialTablePresetExp;
   };
/* #line 25313 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgSymbolStorageClass  : public SgNodeStorageClass
   {

    protected: 


/* #line 25328 "../../../src/frontend/SageIII//StorageClasses.h" */

       EasyStorage < AstAttributeMechanism* > storageOf_attributeMechanism;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgSymbol;
   };
/* #line 25348 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgVariableSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgVariableSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25363 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgVariableSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgVariableSymbol;
   };
/* #line 25383 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateVariableSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateVariableSymbolStorageClass  : public SgVariableSymbolStorageClass
   {

    protected: 


/* #line 25398 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateVariableSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateVariableSymbol;
   };
/* #line 25417 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNonrealSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNonrealSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25432 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNonrealSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNonrealSymbol;
   };
/* #line 25452 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25467 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionSymbol;
   };
/* #line 25487 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgMemberFunctionSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgMemberFunctionSymbolStorageClass  : public SgFunctionSymbolStorageClass
   {

    protected: 


/* #line 25502 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgMemberFunctionSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgMemberFunctionSymbol;
   };
/* #line 25521 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateMemberFunctionSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateMemberFunctionSymbolStorageClass  : public SgMemberFunctionSymbolStorageClass
   {

    protected: 


/* #line 25536 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateMemberFunctionSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateMemberFunctionSymbol;
   };
/* #line 25555 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateFunctionSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateFunctionSymbolStorageClass  : public SgFunctionSymbolStorageClass
   {

    protected: 


/* #line 25570 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateFunctionSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateFunctionSymbol;
   };
/* #line 25589 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgRenameSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgRenameSymbolStorageClass  : public SgFunctionSymbolStorageClass
   {

    protected: 


/* #line 25604 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_original_symbol;
      SgNameStorageClass storageOf_new_name;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgRenameSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgRenameSymbol;
   };
/* #line 25625 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgFunctionTypeSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgFunctionTypeSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25640 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgFunctionTypeSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgFunctionTypeSymbol;
   };
/* #line 25661 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgClassSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgClassSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25676 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgClassSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgClassSymbol;
   };
/* #line 25696 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateClassSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateClassSymbolStorageClass  : public SgClassSymbolStorageClass
   {

    protected: 


/* #line 25711 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateClassSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateClassSymbol;
   };
/* #line 25730 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25745 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateSymbol;
   };
/* #line 25765 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEnumSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEnumSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25780 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEnumSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEnumSymbol;
   };
/* #line 25800 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgEnumFieldSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgEnumFieldSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25815 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgEnumFieldSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgEnumFieldSymbol;
   };
/* #line 25835 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTypedefSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTypedefSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25850 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTypedefSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTypedefSymbol;
   };
/* #line 25870 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgTemplateTypedefSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgTemplateTypedefSymbolStorageClass  : public SgTypedefSymbolStorageClass
   {

    protected: 


/* #line 25885 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgTemplateTypedefSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgTemplateTypedefSymbol;
   };
/* #line 25904 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgLabelSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgLabelSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25919 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;
     unsigned long storageOf_fortran_statement;
     unsigned long storageOf_fortran_alternate_return_parameter;
      int storageOf_numeric_label_value;
      SgLabelSymbol::label_type_enum storageOf_label_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgLabelSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgLabelSymbol;
   };
/* #line 25943 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgDefaultSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgDefaultSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25958 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_type;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgDefaultSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgDefaultSymbol;
   };
/* #line 25978 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgNamespaceSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgNamespaceSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 25993 "../../../src/frontend/SageIII//StorageClasses.h" */

      SgNameStorageClass storageOf_name;
     unsigned long storageOf_declaration;
     unsigned long storageOf_aliasDeclaration;
      bool storageOf_isAlias;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgNamespaceSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgNamespaceSymbol;
   };
/* #line 26016 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgIntrinsicSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgIntrinsicSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26031 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgIntrinsicSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgIntrinsicSymbol;
   };
/* #line 26051 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgModuleSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgModuleSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26066 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgModuleSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgModuleSymbol;
   };
/* #line 26086 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgInterfaceSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgInterfaceSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26101 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgInterfaceSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgInterfaceSymbol;
   };
/* #line 26121 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgCommonSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgCommonSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26136 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgCommonSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgCommonSymbol;
   };
/* #line 26156 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAliasSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAliasSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26171 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_alias;
      bool storageOf_isRenamed;
      SgNameStorageClass storageOf_new_name;
       EasyStorage < SgNodePtrList > storageOf_causal_nodes;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAliasSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAliasSymbol;
   };
/* #line 26194 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgJavaLabelSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgJavaLabelSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26209 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgJavaLabelSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgJavaLabelSymbol;
   };
/* #line 26229 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaPackageSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaPackageSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26244 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaPackageSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaPackageSymbol;
   };
/* #line 26264 "../../../src/frontend/SageIII//StorageClasses.h" */



/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */
/* 
   JH (01/01/2006) ROSETTA generated class declarations for SgAdaTaskSymbolStorageClass 
   used for the ast file IO. Do not chance by hand! 
*/
class SgAdaTaskSymbolStorageClass  : public SgSymbolStorageClass
   {

    protected: 


/* #line 26279 "../../../src/frontend/SageIII//StorageClasses.h" */

     unsigned long storageOf_declaration;


/* #line 10 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStorageClassDeclatationMacros.macro" */


    public:
      void pickOutIRNodeData ( SgAdaTaskSymbol* source );
      static void arrangeStaticDataOfEasyStorageClassesInOneBlock ( );
      static void deleteStaticDataOfEasyStorageClasses ( );
      static void pickOutStaticDataMembers ( );
      static void rebuildStaticDataMembers ( );
      static void writeEasyStorageDataToFile (std::ostream& out);
      static void readEasyStorageDataFromFile (std::istream& in);
      static void writeStaticDataToFile (std::ostream& out );
      static void readStaticDataFromFile (std::istream& in );
      friend class SgAdaTaskSymbol;
   };
/* #line 26299 "../../../src/frontend/SageIII//StorageClasses.h" */



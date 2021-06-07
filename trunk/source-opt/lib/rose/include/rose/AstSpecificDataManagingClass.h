/* #line 1 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */
/* JH (01/01/2006) This file is generated using ROSETTA. It should never be 
   manipulated by hand. The generation is located in buildStorageClasses.C!

    This file contains all declarations for the StorageClasses used for the ast file IO.
*/
class SgProject;


class ROSE_DLL_API AstSpecificDataManagingClass 
  {
    private:
     SgProject* rootOfAst;
     int astIndex;
     unsigned long listOfAccumulatedPoolSizes [ 965 + 1 ];
     SgFunctionTypeTable*  SgNode_globalFunctionTypeTable;
     SgTypeTable*  SgNode_globalTypeTable;
     std::map<SgNode*,std::string>  SgNode_globalMangledNameMap;
     std::map<std::string, int>  SgNode_shortMangledNameCache;
     std::map<SgNode*,std::string>  SgNode_globalQualifiedNameMapForNames;
     std::map<SgNode*,std::string>  SgNode_globalQualifiedNameMapForTypes;
     std::map<SgNode*,std::string>  SgNode_globalQualifiedNameMapForTemplateHeaders;
     std::map<SgNode*,std::string>  SgNode_globalTypeNameMap;
     std::map<SgNode*,std::map<SgNode*,std::string> >  SgNode_globalQualifiedNameMapForMapsOfTypes;
     SgNodeSet  SgSymbolTable_aliasSymbolCausalNodeSet;
     bool  SgSymbolTable_force_search_of_base_classes;
     std::map<int, std::string>  Sg_File_Info_fileidtoname_map;
     std::map<std::string, int>  Sg_File_Info_nametofileid_map;
     bool  SgFile_skip_unparse_asm_commands;
     SgTypePtrList  SgUnparse_Info_structureTagProcessingList;
     bool  SgUnparse_Info_forceDefaultConstructorToTriggerError;
     bool  SgUnparse_Info_extern_C_with_braces;
     int  SgGraph_index_counter;
     int  SgGraphNode_index_counter;
     int  SgGraphEdge_index_counter;
     SgTypeUnknown*  SgTypeUnknown_builtin_type;
     SgTypeChar*  SgTypeChar_builtin_type;
     SgTypeSignedChar*  SgTypeSignedChar_builtin_type;
     SgTypeUnsignedChar*  SgTypeUnsignedChar_builtin_type;
     SgTypeShort*  SgTypeShort_builtin_type;
     SgTypeSignedShort*  SgTypeSignedShort_builtin_type;
     SgTypeUnsignedShort*  SgTypeUnsignedShort_builtin_type;
     SgTypeInt*  SgTypeInt_builtin_type;
     SgTypeSignedInt*  SgTypeSignedInt_builtin_type;
     SgTypeUnsignedInt*  SgTypeUnsignedInt_builtin_type;
     SgTypeLong*  SgTypeLong_builtin_type;
     SgTypeSignedLong*  SgTypeSignedLong_builtin_type;
     SgTypeUnsignedLong*  SgTypeUnsignedLong_builtin_type;
     SgTypeVoid*  SgTypeVoid_builtin_type;
     SgTypeGlobalVoid*  SgTypeGlobalVoid_builtin_type;
     SgTypeWchar*  SgTypeWchar_builtin_type;
     SgTypeFloat*  SgTypeFloat_builtin_type;
     SgTypeDouble*  SgTypeDouble_builtin_type;
     SgTypeLongLong*  SgTypeLongLong_builtin_type;
     SgTypeSignedLongLong*  SgTypeSignedLongLong_builtin_type;
     SgTypeUnsignedLongLong*  SgTypeUnsignedLongLong_builtin_type;
     SgTypeSigned128bitInteger*  SgTypeSigned128bitInteger_builtin_type;
     SgTypeUnsigned128bitInteger*  SgTypeUnsigned128bitInteger_builtin_type;
     SgTypeFloat80*  SgTypeFloat80_builtin_type;
     SgTypeLongDouble*  SgTypeLongDouble_builtin_type;
     SgTypeBool*  SgTypeBool_builtin_type;
     SgNamedType*  SgNamedType_builtin_type;
     SgPartialFunctionModifierType*  SgPartialFunctionModifierType_builtin_type;
     SgTypeEllipse*  SgTypeEllipse_builtin_type;
     SgTypeDefault*  SgTypeDefault_builtin_type;
     SgTypeCAFTeam*  SgTypeCAFTeam_builtin_type;
     SgTypeCrayPointer*  SgTypeCrayPointer_builtin_type;
     SgTypeLabel*  SgTypeLabel_builtin_type;
     SgTypeNullptr*  SgTypeNullptr_builtin_type;
     SgTypeMatrix*  SgTypeMatrix_builtin_type;
     SgTypeTuple*  SgTypeTuple_builtin_type;
     SgTypeChar16*  SgTypeChar16_builtin_type;
     SgTypeChar32*  SgTypeChar32_builtin_type;
     SgTypeFloat128*  SgTypeFloat128_builtin_type;

/* #line 16 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */
    public:
     AstSpecificDataManagingClass(SgProject* root);
     AstSpecificDataManagingClass(const AstSpecificDataManagingClassStorageClass& source );
     SgProject* getRootOfAst () const; 
     unsigned long getNumberOfAccumulatedNodes (const int position) const; 
     unsigned long getMemoryPoolSize (const int position) const ;
     unsigned long getTotalNumberOfASTIRNodes () const;
     int get_AstIndex() const ;
     void setStaticDataMembersOfIRNodes ( ) const ;
     SgFunctionTypeTable*  get_SgNode_globalFunctionTypeTable() const;
     SgTypeTable*  get_SgNode_globalTypeTable() const;
     std::map<SgNode*,std::string>  get_SgNode_globalMangledNameMap() const;
     std::map<std::string, int>  get_SgNode_shortMangledNameCache() const;
     std::map<SgNode*,std::string>  get_SgNode_globalQualifiedNameMapForNames() const;
     std::map<SgNode*,std::string>  get_SgNode_globalQualifiedNameMapForTypes() const;
     std::map<SgNode*,std::string>  get_SgNode_globalQualifiedNameMapForTemplateHeaders() const;
     std::map<SgNode*,std::string>  get_SgNode_globalTypeNameMap() const;
     std::map<SgNode*,std::map<SgNode*,std::string> >  get_SgNode_globalQualifiedNameMapForMapsOfTypes() const;
     SgNodeSet  get_SgSymbolTable_aliasSymbolCausalNodeSet() const;
     bool  get_SgSymbolTable_force_search_of_base_classes() const;
     std::map<int, std::string>  get_Sg_File_Info_fileidtoname_map() const;
     std::map<std::string, int>  get_Sg_File_Info_nametofileid_map() const;
     bool  get_SgFile_skip_unparse_asm_commands() const;
     SgTypePtrList  get_SgUnparse_Info_structureTagProcessingList() const;
     bool  get_SgUnparse_Info_forceDefaultConstructorToTriggerError() const;
     bool  get_SgUnparse_Info_extern_C_with_braces() const;
     int  get_SgGraph_index_counter() const;
     int  get_SgGraphNode_index_counter() const;
     int  get_SgGraphEdge_index_counter() const;
     SgTypeUnknown*  get_SgTypeUnknown_builtin_type() const;
     SgTypeChar*  get_SgTypeChar_builtin_type() const;
     SgTypeSignedChar*  get_SgTypeSignedChar_builtin_type() const;
     SgTypeUnsignedChar*  get_SgTypeUnsignedChar_builtin_type() const;
     SgTypeShort*  get_SgTypeShort_builtin_type() const;
     SgTypeSignedShort*  get_SgTypeSignedShort_builtin_type() const;
     SgTypeUnsignedShort*  get_SgTypeUnsignedShort_builtin_type() const;
     SgTypeInt*  get_SgTypeInt_builtin_type() const;
     SgTypeSignedInt*  get_SgTypeSignedInt_builtin_type() const;
     SgTypeUnsignedInt*  get_SgTypeUnsignedInt_builtin_type() const;
     SgTypeLong*  get_SgTypeLong_builtin_type() const;
     SgTypeSignedLong*  get_SgTypeSignedLong_builtin_type() const;
     SgTypeUnsignedLong*  get_SgTypeUnsignedLong_builtin_type() const;
     SgTypeVoid*  get_SgTypeVoid_builtin_type() const;
     SgTypeGlobalVoid*  get_SgTypeGlobalVoid_builtin_type() const;
     SgTypeWchar*  get_SgTypeWchar_builtin_type() const;
     SgTypeFloat*  get_SgTypeFloat_builtin_type() const;
     SgTypeDouble*  get_SgTypeDouble_builtin_type() const;
     SgTypeLongLong*  get_SgTypeLongLong_builtin_type() const;
     SgTypeSignedLongLong*  get_SgTypeSignedLongLong_builtin_type() const;
     SgTypeUnsignedLongLong*  get_SgTypeUnsignedLongLong_builtin_type() const;
     SgTypeSigned128bitInteger*  get_SgTypeSigned128bitInteger_builtin_type() const;
     SgTypeUnsigned128bitInteger*  get_SgTypeUnsigned128bitInteger_builtin_type() const;
     SgTypeFloat80*  get_SgTypeFloat80_builtin_type() const;
     SgTypeLongDouble*  get_SgTypeLongDouble_builtin_type() const;
     SgTypeBool*  get_SgTypeBool_builtin_type() const;
     SgNamedType*  get_SgNamedType_builtin_type() const;
     SgPartialFunctionModifierType*  get_SgPartialFunctionModifierType_builtin_type() const;
     SgTypeEllipse*  get_SgTypeEllipse_builtin_type() const;
     SgTypeDefault*  get_SgTypeDefault_builtin_type() const;
     SgTypeCAFTeam*  get_SgTypeCAFTeam_builtin_type() const;
     SgTypeCrayPointer*  get_SgTypeCrayPointer_builtin_type() const;
     SgTypeLabel*  get_SgTypeLabel_builtin_type() const;
     SgTypeNullptr*  get_SgTypeNullptr_builtin_type() const;
     SgTypeMatrix*  get_SgTypeMatrix_builtin_type() const;
     SgTypeTuple*  get_SgTypeTuple_builtin_type() const;
     SgTypeChar16*  get_SgTypeChar16_builtin_type() const;
     SgTypeChar32*  get_SgTypeChar32_builtin_type() const;
     SgTypeFloat128*  get_SgTypeFloat128_builtin_type() const;

/* #line 26 "/space/scratch/garyb/SCM/rose/src/ROSETTA/Grammar/grammarStaticDataManagingClassHeader.macro" */

    friend class AST_FILE_IO;
    friend class AstSpecificDataManagingClassStorageClass;
  };



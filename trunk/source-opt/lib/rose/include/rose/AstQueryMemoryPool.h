template <class FunctionalType>
void AstQueryNamespace::queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery, VariantVector* variantsToTraverse)
  {
for (VariantVector::iterator it = variantsToTraverse->begin(); it != variantsToTraverse->end(); ++it)
  {
switch(*it){
 case V_SgName: {
  SgName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSymbolTable: {
  SgSymbolTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPragma: {
  SgPragma::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifierNodes: {
  SgModifierNodes::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConstVolatileModifier: {
  SgConstVolatileModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStorageModifier: {
  SgStorageModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAccessModifier: {
  SgAccessModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionModifier: {
  SgFunctionModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUPC_AccessModifier: {
  SgUPC_AccessModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLinkageModifier: {
  SgLinkageModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSpecialFunctionModifier: {
  SgSpecialFunctionModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeModifier: {
  SgTypeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElaboratedTypeModifier: {
  SgElaboratedTypeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBaseClassModifier: {
  SgBaseClassModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclarationModifier: {
  SgDeclarationModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStructureModifier: {
  SgStructureModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOpenclAccessModeModifier: {
  SgOpenclAccessModeModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifier: {
  SgModifier::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaRangeConstraint: {
  SgAdaRangeConstraint::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTypeConstraint: {
  SgAdaTypeConstraint::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_Sg_File_Info: {
  Sg_File_Info::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSourceFile: {
  SgSourceFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnknownFile: {
  SgUnknownFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFile: {
  SgFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFileList: {
  SgFileList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectory: {
  SgDirectory::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectoryList: {
  SgDirectoryList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProject: {
  SgProject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOptions: {
  SgOptions::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnparse_Info: {
  SgUnparse_Info::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncludeFile: {
  SgIncludeFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFuncDecl_attr: {
  SgFuncDecl_attr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDecl_attr: {
  SgClassDecl_attr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefSeq: {
  SgTypedefSeq::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterTypeList: {
  SgFunctionParameterTypeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateParameter: {
  SgTemplateParameter::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateArgument: {
  SgTemplateArgument::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateParameterList: {
  SgTemplateParameterList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateArgumentList: {
  SgTemplateArgumentList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitAttribute: {
  SgBitAttribute::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAttribute: {
  SgAttribute::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExpBaseClass: {
  SgExpBaseClass::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonrealBaseClass: {
  SgNonrealBaseClass::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBaseClass: {
  SgBaseClass::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUndirectedGraphEdge: {
  SgUndirectedGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDirectedGraphEdge: {
  SgDirectedGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphNode: {
  SgGraphNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphEdge: {
  SgGraphEdge::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStringKeyedBidirectionalGraph: {
  SgStringKeyedBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntKeyedBidirectionalGraph: {
  SgIntKeyedBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBidirectionalGraph: {
  SgBidirectionalGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncidenceDirectedGraph: {
  SgIncidenceDirectedGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncidenceUndirectedGraph: {
  SgIncidenceUndirectedGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraph: {
  SgGraph::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphNodeList: {
  SgGraphNodeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGraphEdgeList: {
  SgGraphEdgeList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgQualifiedName: {
  SgQualifiedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNameGroup: {
  SgNameGroup::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDimensionObject: {
  SgDimensionObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementGroup: {
  SgDataStatementGroup::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementObject: {
  SgDataStatementObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDataStatementValue: {
  SgDataStatementValue::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatItem: {
  SgFormatItem::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatItemList: {
  SgFormatItemList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeTable: {
  SgTypeTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaImportStatementList: {
  SgJavaImportStatementList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaClassDeclarationList: {
  SgJavaClassDeclarationList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgHeaderFileReport: {
  SgHeaderFileReport::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSupport: {
  SgSupport::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnknown: {
  SgTypeUnknown::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeChar: {
  SgTypeChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedChar: {
  SgTypeSignedChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedChar: {
  SgTypeUnsignedChar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeShort: {
  SgTypeShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedShort: {
  SgTypeSignedShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedShort: {
  SgTypeUnsignedShort::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeInt: {
  SgTypeInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedInt: {
  SgTypeSignedInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedInt: {
  SgTypeUnsignedInt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLong: {
  SgTypeLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedLong: {
  SgTypeSignedLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedLong: {
  SgTypeUnsignedLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeVoid: {
  SgTypeVoid::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeGlobalVoid: {
  SgTypeGlobalVoid::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeWchar: {
  SgTypeWchar::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeFloat: {
  SgTypeFloat::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeDouble: {
  SgTypeDouble::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLongLong: {
  SgTypeLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSignedLongLong: {
  SgTypeSignedLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsignedLongLong: {
  SgTypeUnsignedLongLong::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLongDouble: {
  SgTypeLongDouble::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeFloat80: {
  SgTypeFloat80::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeFloat128: {
  SgTypeFloat128::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeString: {
  SgTypeString::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeBool: {
  SgTypeBool::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeFixed: {
  SgTypeFixed::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeMatrix: {
  SgTypeMatrix::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeTuple: {
  SgTypeTuple::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeNullptr: {
  SgTypeNullptr::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeComplex: {
  SgTypeComplex::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeImaginary: {
  SgTypeImaginary::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeDefault: {
  SgTypeDefault::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerMemberType: {
  SgPointerMemberType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReferenceType: {
  SgReferenceType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRvalueReferenceType: {
  SgRvalueReferenceType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclType: {
  SgDeclType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeOfType: {
  SgTypeOfType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeCAFTeam: {
  SgTypeCAFTeam::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeUnsigned128bitInteger: {
  SgTypeUnsigned128bitInteger::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeSigned128bitInteger: {
  SgTypeSigned128bitInteger::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeLabel: {
  SgTypeLabel::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaParameterizedType: {
  SgJavaParameterizedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaQualifiedType: {
  SgJavaQualifiedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaWildcardType: {
  SgJavaWildcardType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaUnionType: {
  SgJavaUnionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaParameterType: {
  SgJavaParameterType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialTableType: {
  SgJovialTableType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateType: {
  SgTemplateType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumType: {
  SgEnumType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefType: {
  SgTypedefType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonrealType: {
  SgNonrealType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAutoType: {
  SgAutoType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModifierType: {
  SgModifierType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPartialFunctionModifierType: {
  SgPartialFunctionModifierType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrayType: {
  SgArrayType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeEllipse: {
  SgTypeEllipse::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaAccessType: {
  SgAdaAccessType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaSubtype: {
  SgAdaSubtype::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaFloatType: {
  SgAdaFloatType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialBitType: {
  SgJovialBitType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeCrayPointer: {
  SgTypeCrayPointer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPartialFunctionType: {
  SgPartialFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionType: {
  SgMemberFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionType: {
  SgFunctionType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerType: {
  SgPointerType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskType: {
  SgAdaTaskType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassType: {
  SgClassType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamedType: {
  SgNamedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgQualifiedNameType: {
  SgQualifiedNameType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeChar16: {
  SgTypeChar16::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeChar32: {
  SgTypeChar32::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgType: {
  SgType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForStatement: {
  SgForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForInitStatement: {
  SgForInitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRangeBasedForStatement: {
  SgRangeBasedForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCatchStatementSeq: {
  SgCatchStatementSeq::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterList: {
  SgFunctionParameterList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCtorInitializerList: {
  SgCtorInitializerList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBasicBlock: {
  SgBasicBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGlobal: {
  SgGlobal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfStmt: {
  SgIfStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWhileStmt: {
  SgWhileStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDoWhileStmt: {
  SgDoWhileStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSwitchStatement: {
  SgSwitchStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCatchOptionStmt: {
  SgCatchOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterScope: {
  SgFunctionParameterScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclarationScope: {
  SgDeclarationScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableDefinition: {
  SgVariableDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStmtDeclarationStatement: {
  SgStmtDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumDeclaration: {
  SgEnumDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmStmt: {
  SgAsmStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionTypeTable: {
  SgFunctionTypeTable::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExprStatement: {
  SgExprStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelStatement: {
  SgLabelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCaseOptionStmt: {
  SgCaseOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTryStmt: {
  SgTryStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefaultOptionStmt: {
  SgDefaultOptionStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBreakStmt: {
  SgBreakStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgContinueStmt: {
  SgContinueStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReturnStmt: {
  SgReturnStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGotoStatement: {
  SgGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaExitStmt: {
  SgAdaExitStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaLoopStmt: {
  SgAdaLoopStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSpawnStmt: {
  SgSpawnStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateTypedefDeclaration: {
  SgTemplateTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationTypedefDeclaration: {
  SgTemplateInstantiationTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefDeclaration: {
  SgTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullStatement: {
  SgNullStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariantStatement: {
  SgVariantStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPragmaDeclaration: {
  SgPragmaDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateClassDeclaration: {
  SgTemplateClassDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateMemberFunctionDeclaration: {
  SgTemplateMemberFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateFunctionDeclaration: {
  SgTemplateFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateVariableDeclaration: {
  SgTemplateVariableDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateDeclaration: {
  SgTemplateDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableDeclaration: {
  SgVariableDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDecl: {
  SgTemplateInstantiationDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDefn: {
  SgTemplateInstantiationDefn::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationFunctionDecl: {
  SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationMemberFunctionDecl: {
  SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonrealDecl: {
  SgNonrealDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWithStatement: {
  SgWithStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPythonGlobalStmt: {
  SgPythonGlobalStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPythonPrintStmt: {
  SgPythonPrintStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPassStatement: {
  SgPassStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssertStmt: {
  SgAssertStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExecStatement: {
  SgExecStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProgramHeaderStatement: {
  SgProgramHeaderStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProcedureHeaderStatement: {
  SgProcedureHeaderStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEntryStatement: {
  SgEntryStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranNonblockedDo: {
  SgFortranNonblockedDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceStatement: {
  SgInterfaceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgParameterStatement: {
  SgParameterStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonBlock: {
  SgCommonBlock::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModuleStatement: {
  SgModuleStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUseStatement: {
  SgUseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgProcessControlStatement: {
  SgProcessControlStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPrintStatement: {
  SgPrintStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReadStatement: {
  SgReadStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWriteStatement: {
  SgWriteStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOpenStatement: {
  SgOpenStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCloseStatement: {
  SgCloseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInquireStatement: {
  SgInquireStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFlushStatement: {
  SgFlushStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBackspaceStatement: {
  SgBackspaceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRewindStatement: {
  SgRewindStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEndfileStatement: {
  SgEndfileStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWaitStatement: {
  SgWaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCAFWithTeamStatement: {
  SgCAFWithTeamStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFormatStatement: {
  SgFormatStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranDo: {
  SgFortranDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgForAllStatement: {
  SgForAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIOStatement: {
  SgIOStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSyncAllStatement: {
  SgSyncAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSyncImagesStatement: {
  SgSyncImagesStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSyncMemoryStatement: {
  SgSyncMemoryStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSyncTeamStatement: {
  SgSyncTeamStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLockStatement: {
  SgLockStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnlockStatement: {
  SgUnlockStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImageControlStatement: {
  SgImageControlStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialCompoolStatement: {
  SgJovialCompoolStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialDefineDeclaration: {
  SgJovialDefineDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialDirectiveStatement: {
  SgJovialDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialOverlayDeclaration: {
  SgJovialOverlayDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialForThenStatement: {
  SgJovialForThenStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMatlabForStatement: {
  SgMatlabForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcNotifyStatement: {
  SgUpcNotifyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcWaitStatement: {
  SgUpcWaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcBarrierStatement: {
  SgUpcBarrierStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcFenceStatement: {
  SgUpcFenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcForAllStatement: {
  SgUpcForAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpParallelStatement: {
  SgOmpParallelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSingleStatement: {
  SgOmpSingleStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTaskStatement: {
  SgOmpTaskStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpForStatement: {
  SgOmpForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpForSimdStatement: {
  SgOmpForSimdStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDoStatement: {
  SgOmpDoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSectionsStatement: {
  SgOmpSectionsStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpAtomicStatement: {
  SgOmpAtomicStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTargetStatement: {
  SgOmpTargetStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTargetDataStatement: {
  SgOmpTargetDataStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSimdStatement: {
  SgOmpSimdStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpClauseBodyStatement: {
  SgOmpClauseBodyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpMasterStatement: {
  SgOmpMasterStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSectionStatement: {
  SgOmpSectionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpOrderedStatement: {
  SgOmpOrderedStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpWorkshareStatement: {
  SgOmpWorkshareStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCriticalStatement: {
  SgOmpCriticalStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpBodyStatement: {
  SgOmpBodyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaThrowStatement: {
  SgJavaThrowStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaForEachStatement: {
  SgJavaForEachStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaSynchronizedStatement: {
  SgJavaSynchronizedStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaLabelStatement: {
  SgJavaLabelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaImportStatement: {
  SgJavaImportStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaPackageDeclaration: {
  SgJavaPackageDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaPackageStatement: {
  SgJavaPackageStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsyncStmt: {
  SgAsyncStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFinishStmt: {
  SgFinishStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAtStmt: {
  SgAtStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAtomicStmt: {
  SgAtomicStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWhenStmt: {
  SgWhenStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBlockDataStatement: {
  SgBlockDataStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImplicitStatement: {
  SgImplicitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatementFunctionStatement: {
  SgStatementFunctionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWhereStatement: {
  SgWhereStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullifyStatement: {
  SgNullifyStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEquivalenceStatement: {
  SgEquivalenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDerivedTypeStatement: {
  SgDerivedTypeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialTableStatement: {
  SgJovialTableStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAttributeSpecificationStatement: {
  SgAttributeSpecificationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAllocateStatement: {
  SgAllocateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeallocateStatement: {
  SgDeallocateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgContainsStatement: {
  SgContainsStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSequenceStatement: {
  SgSequenceStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseWhereStatement: {
  SgElseWhereStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArithmeticIfStatement: {
  SgArithmeticIfStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignStatement: {
  SgAssignStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgComputedGotoStatement: {
  SgComputedGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignedGotoStatement: {
  SgAssignedGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamelistStatement: {
  SgNamelistStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImportStatement: {
  SgImportStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssociateStatement: {
  SgAssociateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFortranIncludeLine: {
  SgFortranIncludeLine::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceDeclarationStatement: {
  SgNamespaceDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceAliasDeclarationStatement: {
  SgNamespaceAliasDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceDefinitionStatement: {
  SgNamespaceDefinitionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUsingDeclarationStatement: {
  SgUsingDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUsingDirectiveStatement: {
  SgUsingDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateInstantiationDirectiveStatement: {
  SgTemplateInstantiationDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStaticAssertionDeclaration: {
  SgStaticAssertionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateClassDefinition: {
  SgTemplateClassDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateFunctionDefinition: {
  SgTemplateFunctionDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDeclaration: {
  SgClassDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassDefinition: {
  SgClassDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionDefinition: {
  SgFunctionDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaAcceptStmt: {
  SgAdaAcceptStmt::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaPackageBody: {
  SgAdaPackageBody::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaPackageSpec: {
  SgAdaPackageSpec::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskBody: {
  SgAdaTaskBody::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskSpec: {
  SgAdaTaskSpec::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgScopeStatement: {
  SgScopeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionDeclaration: {
  SgMemberFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaEntryDecl: {
  SgAdaEntryDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionDeclaration: {
  SgFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncludeDirectiveStatement: {
  SgIncludeDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefineDirectiveStatement: {
  SgDefineDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUndefDirectiveStatement: {
  SgUndefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfdefDirectiveStatement: {
  SgIfdefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfndefDirectiveStatement: {
  SgIfndefDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIfDirectiveStatement: {
  SgIfDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeadIfDirectiveStatement: {
  SgDeadIfDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseDirectiveStatement: {
  SgElseDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElseifDirectiveStatement: {
  SgElseifDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEndifDirectiveStatement: {
  SgEndifDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLineDirectiveStatement: {
  SgLineDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWarningDirectiveStatement: {
  SgWarningDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgErrorDirectiveStatement: {
  SgErrorDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEmptyDirectiveStatement: {
  SgEmptyDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIncludeNextDirectiveStatement: {
  SgIncludeNextDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIdentDirectiveStatement: {
  SgIdentDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLinemarkerDirectiveStatement: {
  SgLinemarkerDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgC_PreprocessorDirectiveStatement: {
  SgC_PreprocessorDirectiveStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageStartStatement: {
  SgClinkageStartStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageEndStatement: {
  SgClinkageEndStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClinkageDeclarationStatement: {
  SgClinkageDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpFlushStatement: {
  SgOmpFlushStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDeclareSimdStatement: {
  SgOmpDeclareSimdStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpBarrierStatement: {
  SgOmpBarrierStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpTaskwaitStatement: {
  SgOmpTaskwaitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpThreadprivateStatement: {
  SgOmpThreadprivateStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMicrosoftAttributeDeclaration: {
  SgMicrosoftAttributeDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEmptyDeclaration: {
  SgEmptyDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaPackageSpecDecl: {
  SgAdaPackageSpecDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaPackageBodyDecl: {
  SgAdaPackageBodyDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskSpecDecl: {
  SgAdaTaskSpecDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskTypeDecl: {
  SgAdaTaskTypeDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskBodyDecl: {
  SgAdaTaskBodyDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaRenamingDecl: {
  SgAdaRenamingDecl::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeclarationStatement: {
  SgDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatement: {
  SgStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarRefExp: {
  SgVarRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonrealRefExp: {
  SgNonrealRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskRefExp: {
  SgAdaTaskRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCompoundLiteralExp: {
  SgCompoundLiteralExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelRefExp: {
  SgLabelRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassNameRefExp: {
  SgClassNameRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionRefExp: {
  SgFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionRefExp: {
  SgMemberFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateFunctionRefExp: {
  SgTemplateFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateMemberFunctionRefExp: {
  SgTemplateMemberFunctionRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSizeOfOp: {
  SgSizeOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAlignOfOp: {
  SgAlignOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNoexceptOp: {
  SgNoexceptOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaInstanceOfOp: {
  SgJavaInstanceOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaMarkerAnnotation: {
  SgJavaMarkerAnnotation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaSingleMemberAnnotation: {
  SgJavaSingleMemberAnnotation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaNormalAnnotation: {
  SgJavaNormalAnnotation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaAnnotation: {
  SgJavaAnnotation::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaTypeExpression: {
  SgJavaTypeExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeExpression: {
  SgTypeExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLambdaExp: {
  SgLambdaExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcLocalsizeofExpression: {
  SgUpcLocalsizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcBlocksizeofExpression: {
  SgUpcBlocksizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcElemsizeofExpression: {
  SgUpcElemsizeofExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgStartOp: {
  SgVarArgStartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgStartOneOperandOp: {
  SgVarArgStartOneOperandOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgOp: {
  SgVarArgOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgEndOp: {
  SgVarArgEndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVarArgCopyOp: {
  SgVarArgCopyOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeIdOp: {
  SgTypeIdOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConditionalExp: {
  SgConditionalExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNewExp: {
  SgNewExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDeleteExp: {
  SgDeleteExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgThisExp: {
  SgThisExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSuperExp: {
  SgSuperExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassExp: {
  SgClassExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRefExp: {
  SgRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAggregateInitializer: {
  SgAggregateInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCompoundInitializer: {
  SgCompoundInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConstructorInitializer: {
  SgConstructorInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignInitializer: {
  SgAssignInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBracedInitializer: {
  SgBracedInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExpressionRoot: {
  SgExpressionRoot::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusOp: {
  SgMinusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnaryAddOp: {
  SgUnaryAddOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNotOp: {
  SgNotOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerDerefExp: {
  SgPointerDerefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAddressOfOp: {
  SgAddressOfOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusMinusOp: {
  SgMinusMinusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPlusPlusOp: {
  SgPlusPlusOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitComplementOp: {
  SgBitComplementOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRealPartOp: {
  SgRealPartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImagPartOp: {
  SgImagPartOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConjugateOp: {
  SgConjugateOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCastExp: {
  SgCastExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgThrowOp: {
  SgThrowOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrowExp: {
  SgArrowExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDotExp: {
  SgDotExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDotStarOp: {
  SgDotStarOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgArrowStarOp: {
  SgArrowStarOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEqualityOp: {
  SgEqualityOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLessThanOp: {
  SgLessThanOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGreaterThanOp: {
  SgGreaterThanOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNotEqualOp: {
  SgNotEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLessOrEqualOp: {
  SgLessOrEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgGreaterOrEqualOp: {
  SgGreaterOrEqualOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAddOp: {
  SgAddOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSubtractOp: {
  SgSubtractOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMultiplyOp: {
  SgMultiplyOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDivideOp: {
  SgDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntegerDivideOp: {
  SgIntegerDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModOp: {
  SgModOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAndOp: {
  SgAndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOrOp: {
  SgOrOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitXorOp: {
  SgBitXorOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitAndOp: {
  SgBitAndOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitOrOp: {
  SgBitOrOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBitEqvOp: {
  SgBitEqvOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommaOpExp: {
  SgCommaOpExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLshiftOp: {
  SgLshiftOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRshiftOp: {
  SgRshiftOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaUnsignedRshiftOp: {
  SgJavaUnsignedRshiftOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPntrArrRefExp: {
  SgPntrArrRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgScopeOp: {
  SgScopeOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAssignOp: {
  SgAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPlusAssignOp: {
  SgPlusAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMinusAssignOp: {
  SgMinusAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAndAssignOp: {
  SgAndAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIorAssignOp: {
  SgIorAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMultAssignOp: {
  SgMultAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDivAssignOp: {
  SgDivAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModAssignOp: {
  SgModAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgXorAssignOp: {
  SgXorAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLshiftAssignOp: {
  SgLshiftAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRshiftAssignOp: {
  SgRshiftAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaUnsignedRshiftAssignOp: {
  SgJavaUnsignedRshiftAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntegerDivideAssignOp: {
  SgIntegerDivideAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExponentiationAssignOp: {
  SgExponentiationAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgConcatenationOp: {
  SgConcatenationOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgReplicationOp: {
  SgReplicationOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAtOp: {
  SgAtOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRemOp: {
  SgRemOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAbsOp: {
  SgAbsOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNaryComparisonOp: {
  SgNaryComparisonOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNaryBooleanOp: {
  SgNaryBooleanOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBoolValExp: {
  SgBoolValExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStringVal: {
  SgStringVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgShortVal: {
  SgShortVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCharVal: {
  SgCharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedCharVal: {
  SgUnsignedCharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgWcharVal: {
  SgWcharVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgChar16Val: {
  SgChar16Val::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgChar32Val: {
  SgChar32Val::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedShortVal: {
  SgUnsignedShortVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntVal: {
  SgIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumVal: {
  SgEnumVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedIntVal: {
  SgUnsignedIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongIntVal: {
  SgLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongLongIntVal: {
  SgLongLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedLongLongIntVal: {
  SgUnsignedLongLongIntVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnsignedLongVal: {
  SgUnsignedLongVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFloatVal: {
  SgFloatVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDoubleVal: {
  SgDoubleVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLongDoubleVal: {
  SgLongDoubleVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFloat80Val: {
  SgFloat80Val::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFloat128Val: {
  SgFloat128Val::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaFloatVal: {
  SgAdaFloatVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialBitVal: {
  SgJovialBitVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullptrValExp: {
  SgNullptrValExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVoidVal: {
  SgVoidVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionParameterRefExp: {
  SgFunctionParameterRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateParameterVal: {
  SgTemplateParameterVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcThreads: {
  SgUpcThreads::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUpcMythread: {
  SgUpcMythread::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgComplexVal: {
  SgComplexVal::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNullExpression: {
  SgNullExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariantExpression: {
  SgVariantExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStatementExpression: {
  SgStatementExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsmOp: {
  SgAsmOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCudaKernelExecConfig: {
  SgCudaKernelExecConfig::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCudaKernelCallExp: {
  SgCudaKernelCallExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLambdaRefExp: {
  SgLambdaRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTupleExp: {
  SgTupleExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgListExp: {
  SgListExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDictionaryExp: {
  SgDictionaryExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgKeyDatumPair: {
  SgKeyDatumPair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgComprehension: {
  SgComprehension::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgListComprehension: {
  SgListComprehension::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSetComprehension: {
  SgSetComprehension::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDictionaryComprehension: {
  SgDictionaryComprehension::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMembershipOp: {
  SgMembershipOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonMembershipOp: {
  SgNonMembershipOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIsOp: {
  SgIsOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIsNotOp: {
  SgIsNotOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgStringConversion: {
  SgStringConversion::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgYieldExpression: {
  SgYieldExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFoldExpression: {
  SgFoldExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgChooseExpression: {
  SgChooseExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAwaitExpression: {
  SgAwaitExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSpaceshipOp: {
  SgSpaceshipOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgHereExp: {
  SgHereExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDotDotExp: {
  SgDotDotExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAtExp: {
  SgAtExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFinishExp: {
  SgFinishExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSubscriptExpression: {
  SgSubscriptExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgColonShapeExp: {
  SgColonShapeExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAsteriskShapeExp: {
  SgAsteriskShapeExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIOItemExpression: {
  SgIOItemExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgImpliedDo: {
  SgImpliedDo::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExponentiationOp: {
  SgExponentiationOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnknownArrayOrFunctionReference: {
  SgUnknownArrayOrFunctionReference::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgActualArgumentExpression: {
  SgActualArgumentExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUserDefinedBinaryOp: {
  SgUserDefinedBinaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPointerAssignOp: {
  SgPointerAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCAFCoExpression: {
  SgCAFCoExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDesignatedInitializer: {
  SgDesignatedInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJovialTablePresetExp: {
  SgJovialTablePresetExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMatrixExp: {
  SgMatrixExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRangeExp: {
  SgRangeExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMagicColonExp: {
  SgMagicColonExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseMultiplyOp: {
  SgElementwiseMultiplyOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPowerOp: {
  SgPowerOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwisePowerOp: {
  SgElementwisePowerOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseDivideOp: {
  SgElementwiseDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLeftDivideOp: {
  SgLeftDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseLeftDivideOp: {
  SgElementwiseLeftDivideOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseAddOp: {
  SgElementwiseAddOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseSubtractOp: {
  SgElementwiseSubtractOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMatrixTransposeOp: {
  SgMatrixTransposeOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgElementwiseOp: {
  SgElementwiseOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInitializer: {
  SgInitializer::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUserDefinedUnaryOp: {
  SgUserDefinedUnaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgPseudoDestructorRefExp: {
  SgPseudoDestructorRefExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUnaryOp: {
  SgUnaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCompoundAssignOp: {
  SgCompoundAssignOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgBinaryOp: {
  SgBinaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNaryOp: {
  SgNaryOp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgValueExp: {
  SgValueExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExprListExp: {
  SgExprListExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionCallExp: {
  SgFunctionCallExp::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCallExpression: {
  SgCallExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypeTraitBuiltinOperator: {
  SgTypeTraitBuiltinOperator::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgExpression: {
  SgExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateVariableSymbol: {
  SgTemplateVariableSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgVariableSymbol: {
  SgVariableSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionTypeSymbol: {
  SgFunctionTypeSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateClassSymbol: {
  SgTemplateClassSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgClassSymbol: {
  SgClassSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateSymbol: {
  SgTemplateSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumSymbol: {
  SgEnumSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgEnumFieldSymbol: {
  SgEnumFieldSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateTypedefSymbol: {
  SgTemplateTypedefSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTypedefSymbol: {
  SgTypedefSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateFunctionSymbol: {
  SgTemplateFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgTemplateMemberFunctionSymbol: {
  SgTemplateMemberFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLabelSymbol: {
  SgLabelSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaLabelSymbol: {
  SgJavaLabelSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgDefaultSymbol: {
  SgDefaultSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNamespaceSymbol: {
  SgNamespaceSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgIntrinsicSymbol: {
  SgIntrinsicSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgModuleSymbol: {
  SgModuleSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceSymbol: {
  SgInterfaceSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonSymbol: {
  SgCommonSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRenameSymbol: {
  SgRenameSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgMemberFunctionSymbol: {
  SgMemberFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgFunctionSymbol: {
  SgFunctionSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAliasSymbol: {
  SgAliasSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNonrealSymbol: {
  SgNonrealSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaPackageSymbol: {
  SgAdaPackageSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgAdaTaskSymbol: {
  SgAdaTaskSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgSymbol: {
  SgSymbol::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgCommonBlockObject: {
  SgCommonBlockObject::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInitializedName: {
  SgInitializedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLambdaCapture: {
  SgLambdaCapture::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLambdaCaptureList: {
  SgLambdaCaptureList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgJavaMemberValuePair: {
  SgJavaMemberValuePair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpOrderedClause: {
  SgOmpOrderedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpNowaitClause: {
  SgOmpNowaitClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpBeginClause: {
  SgOmpBeginClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpEndClause: {
  SgOmpEndClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpUntiedClause: {
  SgOmpUntiedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpMergeableClause: {
  SgOmpMergeableClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDefaultClause: {
  SgOmpDefaultClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpAtomicClause: {
  SgOmpAtomicClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpProcBindClause: {
  SgOmpProcBindClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpInbranchClause: {
  SgOmpInbranchClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpNotinbranchClause: {
  SgOmpNotinbranchClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCollapseClause: {
  SgOmpCollapseClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpIfClause: {
  SgOmpIfClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpFinalClause: {
  SgOmpFinalClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpPriorityClause: {
  SgOmpPriorityClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpNumThreadsClause: {
  SgOmpNumThreadsClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDeviceClause: {
  SgOmpDeviceClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSafelenClause: {
  SgOmpSafelenClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSimdlenClause: {
  SgOmpSimdlenClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpExpressionClause: {
  SgOmpExpressionClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCopyprivateClause: {
  SgOmpCopyprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpPrivateClause: {
  SgOmpPrivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpFirstprivateClause: {
  SgOmpFirstprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpSharedClause: {
  SgOmpSharedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpCopyinClause: {
  SgOmpCopyinClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpLastprivateClause: {
  SgOmpLastprivateClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpReductionClause: {
  SgOmpReductionClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpDependClause: {
  SgOmpDependClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpMapClause: {
  SgOmpMapClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpLinearClause: {
  SgOmpLinearClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpUniformClause: {
  SgOmpUniformClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpAlignedClause: {
  SgOmpAlignedClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpVariablesClause: {
  SgOmpVariablesClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpScheduleClause: {
  SgOmpScheduleClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgOmpClause: {
  SgOmpClause::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgRenamePair: {
  SgRenamePair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgInterfaceBody: {
  SgInterfaceBody::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgHeaderFileBody: {
  SgHeaderFileBody::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedUnaryOperator: {
  SgUntypedUnaryOperator::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedBinaryOperator: {
  SgUntypedBinaryOperator::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExprListExpression: {
  SgUntypedExprListExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNamedExpression: {
  SgUntypedNamedExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNullExpression: {
  SgUntypedNullExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedValueExpression: {
  SgUntypedValueExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedArrayReferenceExpression: {
  SgUntypedArrayReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedSubscriptExpression: {
  SgUntypedSubscriptExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedOtherExpression: {
  SgUntypedOtherExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFunctionCallOrArrayReferenceExpression: {
  SgUntypedFunctionCallOrArrayReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedReferenceExpression: {
  SgUntypedReferenceExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExpression: {
  SgUntypedExpression::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNameListDeclaration: {
  SgUntypedNameListDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedImplicitDeclaration: {
  SgUntypedImplicitDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedVariableDeclaration: {
  SgUntypedVariableDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedProgramHeaderDeclaration: {
  SgUntypedProgramHeaderDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedSubroutineDeclaration: {
  SgUntypedSubroutineDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedInterfaceDeclaration: {
  SgUntypedInterfaceDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedUseStatement: {
  SgUntypedUseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedInitializedNameListDeclaration: {
  SgUntypedInitializedNameListDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNullDeclaration: {
  SgUntypedNullDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNullStatement: {
  SgUntypedNullStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedLabelStatement: {
  SgUntypedLabelStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedIfStatement: {
  SgUntypedIfStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedCaseStatement: {
  SgUntypedCaseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedLoopStatement: {
  SgUntypedLoopStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedWhileStatement: {
  SgUntypedWhileStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedForStatement: {
  SgUntypedForStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedForAllStatement: {
  SgUntypedForAllStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExitStatement: {
  SgUntypedExitStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedGotoStatement: {
  SgUntypedGotoStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedProcedureCallStatement: {
  SgUntypedProcedureCallStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedReturnStatement: {
  SgUntypedReturnStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExtendedReturnStatement: {
  SgUntypedExtendedReturnStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedStopStatement: {
  SgUntypedStopStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedAcceptStatement: {
  SgUntypedAcceptStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedEntryCallStatement: {
  SgUntypedEntryCallStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedRequeueStatement: {
  SgUntypedRequeueStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedDelayUntilStatement: {
  SgUntypedDelayUntilStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedDelayRelativeStatement: {
  SgUntypedDelayRelativeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTerminateAlternativeStatement: {
  SgUntypedTerminateAlternativeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedSelectiveAcceptStatement: {
  SgUntypedSelectiveAcceptStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTimedEntryCallStatement: {
  SgUntypedTimedEntryCallStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedConditionalEntryCallStatement: {
  SgUntypedConditionalEntryCallStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedAsynchronousSelectStatement: {
  SgUntypedAsynchronousSelectStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedAbortStatement: {
  SgUntypedAbortStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedRaiseStatement: {
  SgUntypedRaiseStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedCodeStatement: {
  SgUntypedCodeStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFunctionDeclaration: {
  SgUntypedFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedModuleDeclaration: {
  SgUntypedModuleDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedSubmoduleDeclaration: {
  SgUntypedSubmoduleDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedBlockDataDeclaration: {
  SgUntypedBlockDataDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedPackageDeclaration: {
  SgUntypedPackageDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTaskDeclaration: {
  SgUntypedTaskDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedStructureDeclaration: {
  SgUntypedStructureDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTypedefDeclaration: {
  SgUntypedTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExceptionDeclaration: {
  SgUntypedExceptionDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExceptionHandlerDeclaration: {
  SgUntypedExceptionHandlerDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedUnitDeclaration: {
  SgUntypedUnitDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedDirectiveDeclaration: {
  SgUntypedDirectiveDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedEnumDeclaration: {
  SgUntypedEnumDeclaration::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedDeclarationStatement: {
  SgUntypedDeclarationStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedAssignmentStatement: {
  SgUntypedAssignmentStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedBlockStatement: {
  SgUntypedBlockStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedExpressionStatement: {
  SgUntypedExpressionStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFunctionCallStatement: {
  SgUntypedFunctionCallStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedImageControlStatement: {
  SgUntypedImageControlStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNamedStatement: {
  SgUntypedNamedStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedOtherStatement: {
  SgUntypedOtherStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFunctionScope: {
  SgUntypedFunctionScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedModuleScope: {
  SgUntypedModuleScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedGlobalScope: {
  SgUntypedGlobalScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedStructureDefinition: {
  SgUntypedStructureDefinition::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedScope: {
  SgUntypedScope::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedStatement: {
  SgUntypedStatement::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedArrayType: {
  SgUntypedArrayType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTableType: {
  SgUntypedTableType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedType: {
  SgUntypedType::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedName: {
  SgUntypedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNameList: {
  SgUntypedNameList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedToken: {
  SgUntypedToken::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTokenPair: {
  SgUntypedTokenPair::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTokenList: {
  SgUntypedTokenList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedTokenPairList: {
  SgUntypedTokenPairList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedAttribute: {
  SgUntypedAttribute::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedInitializedName: {
  SgUntypedInitializedName::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFile: {
  SgUntypedFile::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedStatementList: {
  SgUntypedStatementList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedDeclarationStatementList: {
  SgUntypedDeclarationStatementList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedFunctionDeclarationList: {
  SgUntypedFunctionDeclarationList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedInitializedNameList: {
  SgUntypedInitializedNameList::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgUntypedNode: {
  SgUntypedNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLocatedNodeSupport: {
  SgLocatedNodeSupport::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgToken: {
  SgToken::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgLocatedNode: {
  SgLocatedNode::traverseMemoryPoolNodes(astQuery);
  break;
}
case V_SgNode: {
  SgNode::traverseMemoryPoolNodes(astQuery);
  break;
}
default:
{
  // This is a common error after adding a new IR node (because this function should have been automatically generated).
  std::cout << "Case not implemented in queryMemoryPool(..). Exiting." << std::endl;
  ROSE_ASSERT(false);
  break;
}
}
}

};

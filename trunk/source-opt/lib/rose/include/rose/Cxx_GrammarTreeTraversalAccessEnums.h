// GENERATED HEADER FILE --- DO NOT MODIFY!

enum E_SgSourceFile 
{
SgSourceFile_globalScope, SgSourceFile_package, SgSourceFile_import_list, SgSourceFile_class_list};
enum E_SgUnknownFile 
{
SgUnknownFile_globalScope};
enum E_SgProject 
{
SgProject_fileList_ptr};
enum E_SgExpBaseClass 
{
SgExpBaseClass_base_class, SgExpBaseClass_base_class_exp};
enum E_SgNonrealBaseClass 
{
SgNonrealBaseClass_base_class, SgNonrealBaseClass_base_class_nonreal};
enum E_SgBaseClass 
{
SgBaseClass_base_class};
enum E_SgTemplateParameter 
{
SgTemplateParameter_expression, SgTemplateParameter_defaultExpressionParameter, SgTemplateParameter_templateDeclaration, SgTemplateParameter_defaultTemplateDeclarationParameter, SgTemplateParameter_initializedName};
enum E_SgTemplateArgument 
{
SgTemplateArgument_expression, SgTemplateArgument_templateDeclaration, SgTemplateArgument_initializedName};
enum E_SgDirectory 
{
SgDirectory_fileList, SgDirectory_directoryList};
enum E_SgFileList 
{
SgFileList_listOfFiles};
enum E_SgDirectoryList 
{
SgDirectoryList_listOfDirectories};
enum E_SgTemplateArgumentList 
{
SgTemplateArgumentList_args};
enum E_SgTemplateParameterList 
{
SgTemplateParameterList_args};
enum E_SgIncludeFile 
{
SgIncludeFile_include_file_list};
enum E_SgJavaImportStatementList 
{
SgJavaImportStatementList_java_import_list};
enum E_SgJavaClassDeclarationList 
{
SgJavaClassDeclarationList_java_class_list};
enum E_SgTypeUnknown 
{
SgTypeUnknown_type_kind, SgTypeUnknown_builtin_type};
enum E_SgTypeChar 
{
SgTypeChar_type_kind, SgTypeChar_builtin_type};
enum E_SgTypeSignedChar 
{
SgTypeSignedChar_type_kind, SgTypeSignedChar_builtin_type};
enum E_SgTypeUnsignedChar 
{
SgTypeUnsignedChar_type_kind, SgTypeUnsignedChar_builtin_type};
enum E_SgTypeShort 
{
SgTypeShort_type_kind, SgTypeShort_builtin_type};
enum E_SgTypeSignedShort 
{
SgTypeSignedShort_type_kind, SgTypeSignedShort_builtin_type};
enum E_SgTypeUnsignedShort 
{
SgTypeUnsignedShort_type_kind, SgTypeUnsignedShort_builtin_type};
enum E_SgTypeInt 
{
SgTypeInt_type_kind, SgTypeInt_builtin_type};
enum E_SgTypeSignedInt 
{
SgTypeSignedInt_type_kind, SgTypeSignedInt_builtin_type};
enum E_SgTypeUnsignedInt 
{
SgTypeUnsignedInt_type_kind, SgTypeUnsignedInt_builtin_type};
enum E_SgTypeLong 
{
SgTypeLong_type_kind, SgTypeLong_builtin_type};
enum E_SgTypeSignedLong 
{
SgTypeSignedLong_type_kind, SgTypeSignedLong_builtin_type};
enum E_SgTypeUnsignedLong 
{
SgTypeUnsignedLong_type_kind, SgTypeUnsignedLong_builtin_type};
enum E_SgTypeVoid 
{
SgTypeVoid_type_kind, SgTypeVoid_builtin_type};
enum E_SgTypeGlobalVoid 
{
SgTypeGlobalVoid_type_kind, SgTypeGlobalVoid_builtin_type};
enum E_SgTypeWchar 
{
SgTypeWchar_type_kind, SgTypeWchar_builtin_type};
enum E_SgTypeFloat 
{
SgTypeFloat_type_kind, SgTypeFloat_builtin_type};
enum E_SgTypeDouble 
{
SgTypeDouble_type_kind, SgTypeDouble_builtin_type};
enum E_SgTypeLongLong 
{
SgTypeLongLong_type_kind, SgTypeLongLong_builtin_type};
enum E_SgTypeSignedLongLong 
{
SgTypeSignedLongLong_type_kind, SgTypeSignedLongLong_builtin_type};
enum E_SgTypeUnsignedLongLong 
{
SgTypeUnsignedLongLong_type_kind, SgTypeUnsignedLongLong_builtin_type};
enum E_SgTypeSigned128bitInteger 
{
SgTypeSigned128bitInteger_type_kind, SgTypeSigned128bitInteger_builtin_type};
enum E_SgTypeUnsigned128bitInteger 
{
SgTypeUnsigned128bitInteger_type_kind, SgTypeUnsigned128bitInteger_builtin_type};
enum E_SgTypeFloat80 
{
SgTypeFloat80_type_kind, SgTypeFloat80_builtin_type};
enum E_SgTypeLongDouble 
{
SgTypeLongDouble_type_kind, SgTypeLongDouble_builtin_type};
enum E_SgTypeString 
{
SgTypeString_type_kind, SgTypeString_lengthExpression};
enum E_SgTypeBool 
{
SgTypeBool_type_kind, SgTypeBool_builtin_type};
enum E_SgPointerMemberType 
{
SgPointerMemberType_type_kind, SgPointerMemberType_base_type};
enum E_SgPointerType 
{
SgPointerType_type_kind, SgPointerType_base_type};
enum E_SgReferenceType 
{
SgReferenceType_type_kind};
enum E_SgJavaParameterType 
{
SgJavaParameterType_type_kind, SgJavaParameterType_builtin_type, SgJavaParameterType_declaration};
enum E_SgJovialTableType 
{
SgJovialTableType_type_kind, SgJovialTableType_builtin_type, SgJovialTableType_declaration, SgJovialTableType_dim_info};
enum E_SgClassType 
{
SgClassType_type_kind, SgClassType_builtin_type, SgClassType_declaration};
enum E_SgEnumType 
{
SgEnumType_type_kind, SgEnumType_builtin_type, SgEnumType_declaration};
enum E_SgTypedefType 
{
SgTypedefType_type_kind, SgTypedefType_builtin_type, SgTypedefType_declaration};
enum E_SgNonrealType 
{
SgNonrealType_type_kind, SgNonrealType_builtin_type, SgNonrealType_declaration};
enum E_SgJavaParameterizedType 
{
SgJavaParameterizedType_type_kind, SgJavaParameterizedType_builtin_type, SgJavaParameterizedType_declaration};
enum E_SgJavaQualifiedType 
{
SgJavaQualifiedType_type_kind, SgJavaQualifiedType_builtin_type, SgJavaQualifiedType_declaration};
enum E_SgJavaWildcardType 
{
SgJavaWildcardType_type_kind, SgJavaWildcardType_builtin_type, SgJavaWildcardType_declaration};
enum E_SgAdaTaskType 
{
SgAdaTaskType_type_kind, SgAdaTaskType_builtin_type, SgAdaTaskType_declaration};
enum E_SgNamedType 
{
SgNamedType_type_kind, SgNamedType_builtin_type, SgNamedType_declaration};
enum E_SgModifierType 
{
SgModifierType_type_kind, SgModifierType_base_type};
enum E_SgPartialFunctionModifierType 
{
SgPartialFunctionModifierType_type_kind, SgPartialFunctionModifierType_return_type, SgPartialFunctionModifierType_orig_return_type, SgPartialFunctionModifierType_builtin_type};
enum E_SgPartialFunctionType 
{
SgPartialFunctionType_type_kind, SgPartialFunctionType_return_type, SgPartialFunctionType_orig_return_type};
enum E_SgMemberFunctionType 
{
SgMemberFunctionType_type_kind, SgMemberFunctionType_return_type, SgMemberFunctionType_orig_return_type};
enum E_SgFunctionType 
{
SgFunctionType_type_kind, SgFunctionType_return_type, SgFunctionType_orig_return_type};
enum E_SgArrayType 
{
SgArrayType_type_kind, SgArrayType_index, SgArrayType_dim_info};
enum E_SgTypeEllipse 
{
SgTypeEllipse_type_kind, SgTypeEllipse_builtin_type};
enum E_SgTemplateType 
{
SgTemplateType_type_kind, SgTemplateType_class_type, SgTemplateType_parent_class_type, SgTemplateType_template_parameter};
enum E_SgQualifiedNameType 
{
SgQualifiedNameType_type_kind};
enum E_SgTypeComplex 
{
SgTypeComplex_type_kind};
enum E_SgTypeImaginary 
{
SgTypeImaginary_type_kind};
enum E_SgTypeDefault 
{
SgTypeDefault_type_kind, SgTypeDefault_builtin_type};
enum E_SgTypeCAFTeam 
{
SgTypeCAFTeam_type_kind, SgTypeCAFTeam_builtin_type};
enum E_SgTypeCrayPointer 
{
SgTypeCrayPointer_type_kind, SgTypeCrayPointer_builtin_type};
enum E_SgTypeLabel 
{
SgTypeLabel_type_kind, SgTypeLabel_builtin_type};
enum E_SgJavaUnionType 
{
SgJavaUnionType_type_kind};
enum E_SgRvalueReferenceType 
{
SgRvalueReferenceType_type_kind};
enum E_SgTypeNullptr 
{
SgTypeNullptr_type_kind, SgTypeNullptr_builtin_type};
enum E_SgDeclType 
{
SgDeclType_type_kind};
enum E_SgTypeOfType 
{
SgTypeOfType_type_kind};
enum E_SgTypeMatrix 
{
SgTypeMatrix_type_kind, SgTypeMatrix_builtin_type};
enum E_SgTypeTuple 
{
SgTypeTuple_type_kind, SgTypeTuple_builtin_type};
enum E_SgTypeChar16 
{
SgTypeChar16_type_kind, SgTypeChar16_builtin_type};
enum E_SgTypeChar32 
{
SgTypeChar32_type_kind, SgTypeChar32_builtin_type};
enum E_SgTypeFloat128 
{
SgTypeFloat128_type_kind, SgTypeFloat128_builtin_type};
enum E_SgTypeFixed 
{
SgTypeFixed_type_kind, SgTypeFixed_scale, SgTypeFixed_fraction};
enum E_SgAutoType 
{
SgAutoType_type_kind};
enum E_SgAdaAccessType 
{
SgAdaAccessType_type_kind};
enum E_SgAdaSubtype 
{
SgAdaSubtype_type_kind};
enum E_SgAdaFloatType 
{
SgAdaFloatType_type_kind};
enum E_SgJovialBitType 
{
SgJovialBitType_type_kind};
enum E_SgType 
{
SgType_type_kind};
enum E_SgCommonBlockObject 
{
SgCommonBlockObject_variable_reference_list};
enum E_SgInitializedName 
{
SgInitializedName_initptr};
enum E_SgJavaMemberValuePair 
{
SgJavaMemberValuePair_value};
enum E_SgOmpOrderedClause 
{
SgOmpOrderedClause_expression};
enum E_SgOmpCollapseClause 
{
SgOmpCollapseClause_expression};
enum E_SgOmpIfClause 
{
SgOmpIfClause_expression};
enum E_SgOmpNumThreadsClause 
{
SgOmpNumThreadsClause_expression};
enum E_SgOmpDeviceClause 
{
SgOmpDeviceClause_expression};
enum E_SgOmpSafelenClause 
{
SgOmpSafelenClause_expression};
enum E_SgOmpSimdlenClause 
{
SgOmpSimdlenClause_expression};
enum E_SgOmpFinalClause 
{
SgOmpFinalClause_expression};
enum E_SgOmpPriorityClause 
{
SgOmpPriorityClause_expression};
enum E_SgOmpExpressionClause 
{
SgOmpExpressionClause_expression};
enum E_SgOmpCopyprivateClause 
{
SgOmpCopyprivateClause_variables};
enum E_SgOmpPrivateClause 
{
SgOmpPrivateClause_variables};
enum E_SgOmpFirstprivateClause 
{
SgOmpFirstprivateClause_variables};
enum E_SgOmpSharedClause 
{
SgOmpSharedClause_variables};
enum E_SgOmpCopyinClause 
{
SgOmpCopyinClause_variables};
enum E_SgOmpLastprivateClause 
{
SgOmpLastprivateClause_variables};
enum E_SgOmpReductionClause 
{
SgOmpReductionClause_variables};
enum E_SgOmpMapClause 
{
SgOmpMapClause_variables};
enum E_SgOmpUniformClause 
{
SgOmpUniformClause_variables};
enum E_SgOmpAlignedClause 
{
SgOmpAlignedClause_variables, SgOmpAlignedClause_alignment};
enum E_SgOmpLinearClause 
{
SgOmpLinearClause_variables, SgOmpLinearClause_step};
enum E_SgOmpDependClause 
{
SgOmpDependClause_variables};
enum E_SgOmpVariablesClause 
{
SgOmpVariablesClause_variables};
enum E_SgOmpScheduleClause 
{
SgOmpScheduleClause_chunk_size};
enum E_SgUntypedUnaryOperator 
{
SgUntypedUnaryOperator_operand};
enum E_SgUntypedBinaryOperator 
{
SgUntypedBinaryOperator_lhs_operand, SgUntypedBinaryOperator_rhs_operand};
enum E_SgUntypedExprListExpression 
{
SgUntypedExprListExpression_expressions};
enum E_SgUntypedArrayReferenceExpression 
{
SgUntypedArrayReferenceExpression_array_subscripts, SgUntypedArrayReferenceExpression_coarray_subscripts};
enum E_SgUntypedSubscriptExpression 
{
SgUntypedSubscriptExpression_lower_bound, SgUntypedSubscriptExpression_upper_bound, SgUntypedSubscriptExpression_stride};
enum E_SgUntypedNamedExpression 
{
SgUntypedNamedExpression_expression};
enum E_SgUntypedNameListDeclaration 
{
SgUntypedNameListDeclaration_names};
enum E_SgUntypedUseStatement 
{
SgUntypedUseStatement_rename_list};
enum E_SgUntypedVariableDeclaration 
{
SgUntypedVariableDeclaration_base_type_declaration, SgUntypedVariableDeclaration_variables};
enum E_SgUntypedProgramHeaderDeclaration 
{
SgUntypedProgramHeaderDeclaration_parameters, SgUntypedProgramHeaderDeclaration_scope, SgUntypedProgramHeaderDeclaration_modifiers, SgUntypedProgramHeaderDeclaration_end_statement};
enum E_SgUntypedSubroutineDeclaration 
{
SgUntypedSubroutineDeclaration_parameters, SgUntypedSubroutineDeclaration_scope, SgUntypedSubroutineDeclaration_modifiers, SgUntypedSubroutineDeclaration_end_statement};
enum E_SgUntypedInterfaceDeclaration 
{
SgUntypedInterfaceDeclaration_parameters, SgUntypedInterfaceDeclaration_scope, SgUntypedInterfaceDeclaration_modifiers, SgUntypedInterfaceDeclaration_end_statement};
enum E_SgUntypedFunctionDeclaration 
{
SgUntypedFunctionDeclaration_parameters, SgUntypedFunctionDeclaration_scope, SgUntypedFunctionDeclaration_modifiers, SgUntypedFunctionDeclaration_end_statement};
enum E_SgUntypedModuleDeclaration 
{
SgUntypedModuleDeclaration_scope, SgUntypedModuleDeclaration_end_statement};
enum E_SgUntypedSubmoduleDeclaration 
{
SgUntypedSubmoduleDeclaration_scope, SgUntypedSubmoduleDeclaration_end_statement};
enum E_SgUntypedBlockDataDeclaration 
{
SgUntypedBlockDataDeclaration_declaration_list, SgUntypedBlockDataDeclaration_end_statement};
enum E_SgUntypedPackageDeclaration 
{
SgUntypedPackageDeclaration_scope};
enum E_SgUntypedStructureDeclaration 
{
SgUntypedStructureDeclaration_definition};
enum E_SgUntypedExceptionHandlerDeclaration 
{
SgUntypedExceptionHandlerDeclaration_statement};
enum E_SgUntypedExceptionDeclaration 
{
SgUntypedExceptionDeclaration_statement};
enum E_SgUntypedTaskDeclaration 
{
SgUntypedTaskDeclaration_scope};
enum E_SgUntypedUnitDeclaration 
{
SgUntypedUnitDeclaration_scope};
enum E_SgUntypedEnumDeclaration 
{
SgUntypedEnumDeclaration_enumerators};
enum E_SgUntypedInitializedNameListDeclaration 
{
SgUntypedInitializedNameListDeclaration_variables};
enum E_SgUntypedAssignmentStatement 
{
SgUntypedAssignmentStatement_lhs_operand, SgUntypedAssignmentStatement_rhs_operand};
enum E_SgUntypedBlockStatement 
{
SgUntypedBlockStatement_scope};
enum E_SgUntypedExpressionStatement 
{
SgUntypedExpressionStatement_statement_expression};
enum E_SgUntypedFunctionCallStatement 
{
SgUntypedFunctionCallStatement_function, SgUntypedFunctionCallStatement_args};
enum E_SgUntypedImageControlStatement 
{
SgUntypedImageControlStatement_variable, SgUntypedImageControlStatement_expression, SgUntypedImageControlStatement_status_list};
enum E_SgUntypedFunctionScope 
{
SgUntypedFunctionScope_declaration_list, SgUntypedFunctionScope_statement_list, SgUntypedFunctionScope_function_list};
enum E_SgUntypedModuleScope 
{
SgUntypedModuleScope_declaration_list, SgUntypedModuleScope_statement_list, SgUntypedModuleScope_function_list};
enum E_SgUntypedGlobalScope 
{
SgUntypedGlobalScope_declaration_list, SgUntypedGlobalScope_statement_list, SgUntypedGlobalScope_function_list};
enum E_SgUntypedStructureDefinition 
{
SgUntypedStructureDefinition_declaration_list, SgUntypedStructureDefinition_statement_list, SgUntypedStructureDefinition_function_list, SgUntypedStructureDefinition_scope};
enum E_SgUntypedScope 
{
SgUntypedScope_declaration_list, SgUntypedScope_statement_list, SgUntypedScope_function_list};
enum E_SgUntypedIfStatement 
{
SgUntypedIfStatement_conditional, SgUntypedIfStatement_true_body, SgUntypedIfStatement_false_body};
enum E_SgUntypedCaseStatement 
{
SgUntypedCaseStatement_expression, SgUntypedCaseStatement_body};
enum E_SgUntypedLabelStatement 
{
SgUntypedLabelStatement_statement};
enum E_SgUntypedWhileStatement 
{
SgUntypedWhileStatement_condition, SgUntypedWhileStatement_body};
enum E_SgUntypedForStatement 
{
SgUntypedForStatement_initialization, SgUntypedForStatement_bound, SgUntypedForStatement_increment, SgUntypedForStatement_body};
enum E_SgUntypedReturnStatement 
{
SgUntypedReturnStatement_expression};
enum E_SgUntypedStopStatement 
{
SgUntypedStopStatement_expression};
enum E_SgUntypedForAllStatement 
{
SgUntypedForAllStatement_iterates, SgUntypedForAllStatement_local, SgUntypedForAllStatement_mask};
enum E_SgUntypedArrayType 
{
SgUntypedArrayType_type_kind, SgUntypedArrayType_modifiers, SgUntypedArrayType_dim_info};
enum E_SgUntypedTableType 
{
SgUntypedTableType_type_kind, SgUntypedTableType_modifiers, SgUntypedTableType_dim_info};
enum E_SgUntypedType 
{
SgUntypedType_type_kind, SgUntypedType_modifiers};
enum E_SgUntypedFile 
{
SgUntypedFile_scope};
enum E_SgUntypedStatementList 
{
SgUntypedStatementList_stmt_list};
enum E_SgUntypedDeclarationStatementList 
{
SgUntypedDeclarationStatementList_decl_list};
enum E_SgUntypedFunctionDeclarationList 
{
SgUntypedFunctionDeclarationList_func_list};
enum E_SgUntypedInitializedNameList 
{
SgUntypedInitializedNameList_name_list};
enum E_SgUntypedNameList 
{
SgUntypedNameList_name_list};
enum E_SgUntypedTokenList 
{
SgUntypedTokenList_token_list};
enum E_SgUntypedTokenPairList 
{
SgUntypedTokenPairList_token_pair_list};
enum E_SgLambdaCapture 
{
SgLambdaCapture_capture_variable, SgLambdaCapture_source_closure_variable, SgLambdaCapture_closure_variable};
enum E_SgLambdaCaptureList 
{
SgLambdaCaptureList_capture_list};
enum E_SgGlobal 
{
SgGlobal_declarations};
enum E_SgBasicBlock 
{
SgBasicBlock_statements};
enum E_SgIfStmt 
{
SgIfStmt_conditional, SgIfStmt_true_body, SgIfStmt_false_body};
enum E_SgForStatement 
{
SgForStatement_for_init_stmt, SgForStatement_test, SgForStatement_increment, SgForStatement_loop_body, SgForStatement_else_body};
enum E_SgTemplateFunctionDefinition 
{
SgTemplateFunctionDefinition_body};
enum E_SgFunctionDefinition 
{
SgFunctionDefinition_body};
enum E_SgTemplateInstantiationDefn 
{
SgTemplateInstantiationDefn_members};
enum E_SgTemplateClassDefinition 
{
SgTemplateClassDefinition_members};
enum E_SgClassDefinition 
{
SgClassDefinition_members};
enum E_SgWhileStmt 
{
SgWhileStmt_condition, SgWhileStmt_body, SgWhileStmt_else_body};
enum E_SgDoWhileStmt 
{
SgDoWhileStmt_body, SgDoWhileStmt_condition};
enum E_SgSwitchStatement 
{
SgSwitchStatement_item_selector, SgSwitchStatement_body};
enum E_SgCatchOptionStmt 
{
SgCatchOptionStmt_condition, SgCatchOptionStmt_body};
enum E_SgNamespaceDefinitionStatement 
{
SgNamespaceDefinitionStatement_declarations};
enum E_SgAssociateStatement 
{
SgAssociateStatement_body};
enum E_SgFortranNonblockedDo 
{
SgFortranNonblockedDo_initialization, SgFortranNonblockedDo_bound, SgFortranNonblockedDo_increment, SgFortranNonblockedDo_body};
enum E_SgFortranDo 
{
SgFortranDo_initialization, SgFortranDo_bound, SgFortranDo_increment, SgFortranDo_body};
enum E_SgForAllStatement 
{
SgForAllStatement_forall_header, SgForAllStatement_body};
enum E_SgUpcForAllStatement 
{
SgUpcForAllStatement_for_init_stmt, SgUpcForAllStatement_test, SgUpcForAllStatement_increment, SgUpcForAllStatement_affinity, SgUpcForAllStatement_loop_body};
enum E_SgCAFWithTeamStatement 
{
SgCAFWithTeamStatement_body};
enum E_SgJavaForEachStatement 
{
SgJavaForEachStatement_element, SgJavaForEachStatement_collection, SgJavaForEachStatement_loop_body};
enum E_SgJavaLabelStatement 
{
SgJavaLabelStatement_statement};
enum E_SgMatlabForStatement 
{
SgMatlabForStatement_index, SgMatlabForStatement_range, SgMatlabForStatement_body};
enum E_SgFunctionParameterScope 
{
SgFunctionParameterScope_declarations};
enum E_SgRangeBasedForStatement 
{
SgRangeBasedForStatement_iterator_declaration, SgRangeBasedForStatement_range_declaration, SgRangeBasedForStatement_begin_declaration, SgRangeBasedForStatement_end_declaration, SgRangeBasedForStatement_not_equal_expression, SgRangeBasedForStatement_increment_expression, SgRangeBasedForStatement_loop_body};
enum E_SgJovialForThenStatement 
{
SgJovialForThenStatement_initialization, SgJovialForThenStatement_while_expression, SgJovialForThenStatement_by_or_then_expression, SgJovialForThenStatement_loop_body};
enum E_SgAdaAcceptStmt 
{
SgAdaAcceptStmt_parameterList, SgAdaAcceptStmt_entry, SgAdaAcceptStmt_index, SgAdaAcceptStmt_body};
enum E_SgAdaPackageSpec 
{
SgAdaPackageSpec_declarations};
enum E_SgAdaPackageBody 
{
SgAdaPackageBody_statements};
enum E_SgAdaTaskSpec 
{
SgAdaTaskSpec_declarations};
enum E_SgAdaTaskBody 
{
SgAdaTaskBody_statements};
enum E_SgFunctionParameterList 
{
SgFunctionParameterList_args};
enum E_SgTemplateVariableDeclaration 
{
SgTemplateVariableDeclaration_baseTypeDefiningDeclaration, SgTemplateVariableDeclaration_variables};
enum E_SgVariableDeclaration 
{
SgVariableDeclaration_baseTypeDefiningDeclaration, SgVariableDeclaration_variables};
enum E_SgVariableDefinition 
{
SgVariableDefinition_vardefn, SgVariableDefinition_bitfield};
enum E_SgEnumDeclaration 
{
SgEnumDeclaration_enumerators};
enum E_SgAsmStmt 
{
SgAsmStmt_operands};
enum E_SgTemplateInstantiationDirectiveStatement 
{
SgTemplateInstantiationDirectiveStatement_declaration};
enum E_SgUseStatement 
{
SgUseStatement_rename_list};
enum E_SgNamespaceDeclarationStatement 
{
SgNamespaceDeclarationStatement_definition};
enum E_SgInterfaceStatement 
{
SgInterfaceStatement_interface_body_list};
enum E_SgCommonBlock 
{
SgCommonBlock_block_list};
enum E_SgTemplateTypedefDeclaration 
{
SgTemplateTypedefDeclaration_declaration};
enum E_SgTemplateInstantiationTypedefDeclaration 
{
SgTemplateInstantiationTypedefDeclaration_declaration};
enum E_SgTypedefDeclaration 
{
SgTypedefDeclaration_declaration};
enum E_SgStatementFunctionStatement 
{
SgStatementFunctionStatement_function, SgStatementFunctionStatement_expression};
enum E_SgCtorInitializerList 
{
SgCtorInitializerList_ctors};
enum E_SgPragmaDeclaration 
{
SgPragmaDeclaration_pragma};
enum E_SgTemplateClassDeclaration 
{
SgTemplateClassDeclaration_definition, SgTemplateClassDeclaration_decoratorList};
enum E_SgTemplateInstantiationDecl 
{
SgTemplateInstantiationDecl_definition, SgTemplateInstantiationDecl_decoratorList};
enum E_SgDerivedTypeStatement 
{
SgDerivedTypeStatement_definition, SgDerivedTypeStatement_decoratorList};
enum E_SgModuleStatement 
{
SgModuleStatement_definition, SgModuleStatement_decoratorList};
enum E_SgJavaPackageDeclaration 
{
SgJavaPackageDeclaration_definition, SgJavaPackageDeclaration_decoratorList};
enum E_SgJovialTableStatement 
{
SgJovialTableStatement_definition, SgJovialTableStatement_decoratorList};
enum E_SgClassDeclaration 
{
SgClassDeclaration_definition, SgClassDeclaration_decoratorList};
enum E_SgImplicitStatement 
{
SgImplicitStatement_variables};
enum E_SgTemplateFunctionDeclaration 
{
SgTemplateFunctionDeclaration_parameterList, SgTemplateFunctionDeclaration_decoratorList, SgTemplateFunctionDeclaration_definition};
enum E_SgTemplateMemberFunctionDeclaration 
{
SgTemplateMemberFunctionDeclaration_parameterList, SgTemplateMemberFunctionDeclaration_decoratorList, SgTemplateMemberFunctionDeclaration_definition, SgTemplateMemberFunctionDeclaration_CtorInitializerList};
enum E_SgTemplateInstantiationMemberFunctionDecl 
{
SgTemplateInstantiationMemberFunctionDecl_parameterList, SgTemplateInstantiationMemberFunctionDecl_decoratorList, SgTemplateInstantiationMemberFunctionDecl_definition, SgTemplateInstantiationMemberFunctionDecl_CtorInitializerList};
enum E_SgMemberFunctionDeclaration 
{
SgMemberFunctionDeclaration_parameterList, SgMemberFunctionDeclaration_decoratorList, SgMemberFunctionDeclaration_definition, SgMemberFunctionDeclaration_CtorInitializerList};
enum E_SgTemplateInstantiationFunctionDecl 
{
SgTemplateInstantiationFunctionDecl_parameterList, SgTemplateInstantiationFunctionDecl_decoratorList, SgTemplateInstantiationFunctionDecl_definition};
enum E_SgProgramHeaderStatement 
{
SgProgramHeaderStatement_parameterList, SgProgramHeaderStatement_decoratorList, SgProgramHeaderStatement_definition};
enum E_SgProcedureHeaderStatement 
{
SgProcedureHeaderStatement_parameterList, SgProcedureHeaderStatement_decoratorList, SgProcedureHeaderStatement_definition, SgProcedureHeaderStatement_result_name};
enum E_SgEntryStatement 
{
SgEntryStatement_parameterList, SgEntryStatement_decoratorList, SgEntryStatement_definition, SgEntryStatement_result_name};
enum E_SgAdaEntryDecl 
{
SgAdaEntryDecl_parameterList, SgAdaEntryDecl_decoratorList, SgAdaEntryDecl_definition};
enum E_SgFunctionDeclaration 
{
SgFunctionDeclaration_parameterList, SgFunctionDeclaration_decoratorList, SgFunctionDeclaration_definition};
enum E_SgIncludeDirectiveStatement 
{
SgIncludeDirectiveStatement_headerFileBody};
enum E_SgOmpThreadprivateStatement 
{
SgOmpThreadprivateStatement_variables};
enum E_SgStmtDeclarationStatement 
{
SgStmtDeclarationStatement_statement};
enum E_SgStaticAssertionDeclaration 
{
SgStaticAssertionDeclaration_condition};
enum E_SgOmpDeclareSimdStatement 
{
SgOmpDeclareSimdStatement_clauses};
enum E_SgJovialOverlayDeclaration 
{
SgJovialOverlayDeclaration_address, SgJovialOverlayDeclaration_overlay};
enum E_SgAdaPackageBodyDecl 
{
SgAdaPackageBodyDecl_definition};
enum E_SgAdaPackageSpecDecl 
{
SgAdaPackageSpecDecl_definition};
enum E_SgAdaTaskSpecDecl 
{
SgAdaTaskSpecDecl_definition};
enum E_SgAdaTaskBodyDecl 
{
SgAdaTaskBodyDecl_definition};
enum E_SgAdaTaskTypeDecl 
{
SgAdaTaskTypeDecl_definition};
enum E_SgExprStatement 
{
SgExprStatement_expression};
enum E_SgLabelStatement 
{
SgLabelStatement_statement};
enum E_SgCaseOptionStmt 
{
SgCaseOptionStmt_key, SgCaseOptionStmt_body, SgCaseOptionStmt_key_range_end};
enum E_SgTryStmt 
{
SgTryStmt_body, SgTryStmt_catch_statement_seq_root, SgTryStmt_else_body, SgTryStmt_finally_body};
enum E_SgDefaultOptionStmt 
{
SgDefaultOptionStmt_body};
enum E_SgReturnStmt 
{
SgReturnStmt_expression};
enum E_SgSpawnStmt 
{
SgSpawnStmt_the_func};
enum E_SgForInitStatement 
{
SgForInitStatement_init_stmt};
enum E_SgCatchStatementSeq 
{
SgCatchStatementSeq_catch_statement_seq};
enum E_SgPrintStatement 
{
SgPrintStatement_io_stmt_list, SgPrintStatement_unit, SgPrintStatement_iostat, SgPrintStatement_err, SgPrintStatement_iomsg, SgPrintStatement_format};
enum E_SgReadStatement 
{
SgReadStatement_io_stmt_list, SgReadStatement_unit, SgReadStatement_iostat, SgReadStatement_err, SgReadStatement_iomsg, SgReadStatement_format, SgReadStatement_namelist, SgReadStatement_advance, SgReadStatement_asynchronous, SgReadStatement_blank, SgReadStatement_decimal, SgReadStatement_delim, SgReadStatement_end, SgReadStatement_eor, SgReadStatement_id, SgReadStatement_pad, SgReadStatement_pos, SgReadStatement_rec, SgReadStatement_round, SgReadStatement_sign, SgReadStatement_size};
enum E_SgWriteStatement 
{
SgWriteStatement_io_stmt_list, SgWriteStatement_unit, SgWriteStatement_iostat, SgWriteStatement_err, SgWriteStatement_iomsg, SgWriteStatement_format, SgWriteStatement_namelist, SgWriteStatement_advance, SgWriteStatement_asynchronous, SgWriteStatement_blank, SgWriteStatement_decimal, SgWriteStatement_delim, SgWriteStatement_end, SgWriteStatement_eor, SgWriteStatement_id, SgWriteStatement_pad, SgWriteStatement_pos, SgWriteStatement_rec, SgWriteStatement_round, SgWriteStatement_sign, SgWriteStatement_size};
enum E_SgOpenStatement 
{
SgOpenStatement_io_stmt_list, SgOpenStatement_unit, SgOpenStatement_iostat, SgOpenStatement_err, SgOpenStatement_iomsg, SgOpenStatement_file, SgOpenStatement_status, SgOpenStatement_access, SgOpenStatement_form, SgOpenStatement_recl, SgOpenStatement_blank, SgOpenStatement_position, SgOpenStatement_action, SgOpenStatement_delim, SgOpenStatement_pad, SgOpenStatement_round, SgOpenStatement_sign, SgOpenStatement_asynchronous};
enum E_SgCloseStatement 
{
SgCloseStatement_io_stmt_list, SgCloseStatement_unit, SgCloseStatement_iostat, SgCloseStatement_err, SgCloseStatement_iomsg, SgCloseStatement_status};
enum E_SgInquireStatement 
{
SgInquireStatement_io_stmt_list, SgInquireStatement_unit, SgInquireStatement_iostat, SgInquireStatement_err, SgInquireStatement_iomsg};
enum E_SgFlushStatement 
{
SgFlushStatement_io_stmt_list, SgFlushStatement_unit, SgFlushStatement_iostat, SgFlushStatement_err, SgFlushStatement_iomsg};
enum E_SgBackspaceStatement 
{
SgBackspaceStatement_io_stmt_list, SgBackspaceStatement_unit, SgBackspaceStatement_iostat, SgBackspaceStatement_err, SgBackspaceStatement_iomsg};
enum E_SgRewindStatement 
{
SgRewindStatement_io_stmt_list, SgRewindStatement_unit, SgRewindStatement_iostat, SgRewindStatement_err, SgRewindStatement_iomsg};
enum E_SgEndfileStatement 
{
SgEndfileStatement_io_stmt_list, SgEndfileStatement_unit, SgEndfileStatement_iostat, SgEndfileStatement_err, SgEndfileStatement_iomsg};
enum E_SgWaitStatement 
{
SgWaitStatement_io_stmt_list, SgWaitStatement_unit, SgWaitStatement_iostat, SgWaitStatement_err, SgWaitStatement_iomsg};
enum E_SgIOStatement 
{
SgIOStatement_io_stmt_list, SgIOStatement_unit, SgIOStatement_iostat, SgIOStatement_err, SgIOStatement_iomsg};
enum E_SgWhereStatement 
{
SgWhereStatement_condition, SgWhereStatement_body, SgWhereStatement_elsewhere};
enum E_SgElseWhereStatement 
{
SgElseWhereStatement_condition, SgElseWhereStatement_body, SgElseWhereStatement_elsewhere};
enum E_SgNullifyStatement 
{
SgNullifyStatement_pointer_list};
enum E_SgArithmeticIfStatement 
{
SgArithmeticIfStatement_conditional};
enum E_SgAssignStatement 
{
SgAssignStatement_value};
enum E_SgComputedGotoStatement 
{
SgComputedGotoStatement_labelList, SgComputedGotoStatement_label_index};
enum E_SgAssignedGotoStatement 
{
SgAssignedGotoStatement_targets};
enum E_SgAllocateStatement 
{
SgAllocateStatement_expr_list, SgAllocateStatement_stat_expression, SgAllocateStatement_errmsg_expression, SgAllocateStatement_source_expression};
enum E_SgDeallocateStatement 
{
SgDeallocateStatement_expr_list, SgDeallocateStatement_stat_expression, SgDeallocateStatement_errmsg_expression};
enum E_SgUpcNotifyStatement 
{
SgUpcNotifyStatement_notify_expression};
enum E_SgUpcWaitStatement 
{
SgUpcWaitStatement_wait_expression};
enum E_SgUpcBarrierStatement 
{
SgUpcBarrierStatement_barrier_expression};
enum E_SgOmpFlushStatement 
{
SgOmpFlushStatement_variables};
enum E_SgOmpMasterStatement 
{
SgOmpMasterStatement_body};
enum E_SgOmpOrderedStatement 
{
SgOmpOrderedStatement_body};
enum E_SgOmpCriticalStatement 
{
SgOmpCriticalStatement_body};
enum E_SgOmpSectionStatement 
{
SgOmpSectionStatement_body};
enum E_SgOmpWorkshareStatement 
{
SgOmpWorkshareStatement_body};
enum E_SgOmpParallelStatement 
{
SgOmpParallelStatement_body, SgOmpParallelStatement_clauses};
enum E_SgOmpSingleStatement 
{
SgOmpSingleStatement_body, SgOmpSingleStatement_clauses};
enum E_SgOmpAtomicStatement 
{
SgOmpAtomicStatement_body, SgOmpAtomicStatement_clauses};
enum E_SgOmpTaskStatement 
{
SgOmpTaskStatement_body, SgOmpTaskStatement_clauses};
enum E_SgOmpForStatement 
{
SgOmpForStatement_body, SgOmpForStatement_clauses};
enum E_SgOmpDoStatement 
{
SgOmpDoStatement_body, SgOmpDoStatement_clauses};
enum E_SgOmpSectionsStatement 
{
SgOmpSectionsStatement_body, SgOmpSectionsStatement_clauses};
enum E_SgOmpTargetStatement 
{
SgOmpTargetStatement_body, SgOmpTargetStatement_clauses};
enum E_SgOmpTargetDataStatement 
{
SgOmpTargetDataStatement_body, SgOmpTargetDataStatement_clauses};
enum E_SgOmpSimdStatement 
{
SgOmpSimdStatement_body, SgOmpSimdStatement_clauses};
enum E_SgOmpForSimdStatement 
{
SgOmpForSimdStatement_body, SgOmpForSimdStatement_clauses};
enum E_SgOmpClauseBodyStatement 
{
SgOmpClauseBodyStatement_body, SgOmpClauseBodyStatement_clauses};
enum E_SgOmpBodyStatement 
{
SgOmpBodyStatement_body};
enum E_SgWithStatement 
{
SgWithStatement_expression, SgWithStatement_body};
enum E_SgPythonPrintStmt 
{
SgPythonPrintStmt_destination, SgPythonPrintStmt_values};
enum E_SgAssertStmt 
{
SgAssertStmt_test, SgAssertStmt_exception_argument};
enum E_SgExecStatement 
{
SgExecStatement_executable, SgExecStatement_globals, SgExecStatement_locals};
enum E_SgPythonGlobalStmt 
{
SgPythonGlobalStmt_names};
enum E_SgJavaSynchronizedStatement 
{
SgJavaSynchronizedStatement_expression, SgJavaSynchronizedStatement_body};
enum E_SgAsyncStmt 
{
SgAsyncStmt_body};
enum E_SgFinishStmt 
{
SgFinishStmt_body};
enum E_SgAtStmt 
{
SgAtStmt_expression, SgAtStmt_body};
enum E_SgAtomicStmt 
{
SgAtomicStmt_body};
enum E_SgWhenStmt 
{
SgWhenStmt_expression, SgWhenStmt_body};
enum E_SgSyncAllStatement 
{
SgSyncAllStatement_stat, SgSyncAllStatement_err_msg, SgSyncAllStatement_acquired_lock};
enum E_SgSyncImagesStatement 
{
SgSyncImagesStatement_stat, SgSyncImagesStatement_err_msg, SgSyncImagesStatement_acquired_lock, SgSyncImagesStatement_image_set};
enum E_SgSyncMemoryStatement 
{
SgSyncMemoryStatement_stat, SgSyncMemoryStatement_err_msg, SgSyncMemoryStatement_acquired_lock};
enum E_SgSyncTeamStatement 
{
SgSyncTeamStatement_stat, SgSyncTeamStatement_err_msg, SgSyncTeamStatement_acquired_lock, SgSyncTeamStatement_team_value};
enum E_SgLockStatement 
{
SgLockStatement_stat, SgLockStatement_err_msg, SgLockStatement_acquired_lock, SgLockStatement_lock_variable};
enum E_SgUnlockStatement 
{
SgUnlockStatement_stat, SgUnlockStatement_err_msg, SgUnlockStatement_acquired_lock, SgUnlockStatement_lock_variable};
enum E_SgImageControlStatement 
{
SgImageControlStatement_stat, SgImageControlStatement_err_msg, SgImageControlStatement_acquired_lock};
enum E_SgAdaExitStmt 
{
SgAdaExitStmt_condition};
enum E_SgAdaLoopStmt 
{
SgAdaLoopStmt_body};
enum E_SgExpressionRoot 
{
SgExpressionRoot_operand_i};
enum E_SgMinusOp 
{
SgMinusOp_operand_i};
enum E_SgUnaryAddOp 
{
SgUnaryAddOp_operand_i};
enum E_SgNotOp 
{
SgNotOp_operand_i};
enum E_SgPointerDerefExp 
{
SgPointerDerefExp_operand_i};
enum E_SgAddressOfOp 
{
SgAddressOfOp_operand_i};
enum E_SgMinusMinusOp 
{
SgMinusMinusOp_operand_i};
enum E_SgPlusPlusOp 
{
SgPlusPlusOp_operand_i};
enum E_SgBitComplementOp 
{
SgBitComplementOp_operand_i};
enum E_SgCastExp 
{
SgCastExp_operand_i};
enum E_SgThrowOp 
{
SgThrowOp_operand_i};
enum E_SgRealPartOp 
{
SgRealPartOp_operand_i};
enum E_SgImagPartOp 
{
SgImagPartOp_operand_i};
enum E_SgConjugateOp 
{
SgConjugateOp_operand_i};
enum E_SgUserDefinedUnaryOp 
{
SgUserDefinedUnaryOp_operand_i};
enum E_SgMatrixTransposeOp 
{
SgMatrixTransposeOp_operand_i};
enum E_SgAbsOp 
{
SgAbsOp_operand_i};
enum E_SgUnaryOp 
{
SgUnaryOp_operand_i};
enum E_SgArrowExp 
{
SgArrowExp_lhs_operand_i, SgArrowExp_rhs_operand_i};
enum E_SgDotExp 
{
SgDotExp_lhs_operand_i, SgDotExp_rhs_operand_i};
enum E_SgDotStarOp 
{
SgDotStarOp_lhs_operand_i, SgDotStarOp_rhs_operand_i};
enum E_SgArrowStarOp 
{
SgArrowStarOp_lhs_operand_i, SgArrowStarOp_rhs_operand_i};
enum E_SgEqualityOp 
{
SgEqualityOp_lhs_operand_i, SgEqualityOp_rhs_operand_i};
enum E_SgLessThanOp 
{
SgLessThanOp_lhs_operand_i, SgLessThanOp_rhs_operand_i};
enum E_SgGreaterThanOp 
{
SgGreaterThanOp_lhs_operand_i, SgGreaterThanOp_rhs_operand_i};
enum E_SgNotEqualOp 
{
SgNotEqualOp_lhs_operand_i, SgNotEqualOp_rhs_operand_i};
enum E_SgLessOrEqualOp 
{
SgLessOrEqualOp_lhs_operand_i, SgLessOrEqualOp_rhs_operand_i};
enum E_SgGreaterOrEqualOp 
{
SgGreaterOrEqualOp_lhs_operand_i, SgGreaterOrEqualOp_rhs_operand_i};
enum E_SgAddOp 
{
SgAddOp_lhs_operand_i, SgAddOp_rhs_operand_i};
enum E_SgSubtractOp 
{
SgSubtractOp_lhs_operand_i, SgSubtractOp_rhs_operand_i};
enum E_SgMultiplyOp 
{
SgMultiplyOp_lhs_operand_i, SgMultiplyOp_rhs_operand_i};
enum E_SgDivideOp 
{
SgDivideOp_lhs_operand_i, SgDivideOp_rhs_operand_i};
enum E_SgIntegerDivideOp 
{
SgIntegerDivideOp_lhs_operand_i, SgIntegerDivideOp_rhs_operand_i};
enum E_SgModOp 
{
SgModOp_lhs_operand_i, SgModOp_rhs_operand_i};
enum E_SgAndOp 
{
SgAndOp_lhs_operand_i, SgAndOp_rhs_operand_i};
enum E_SgOrOp 
{
SgOrOp_lhs_operand_i, SgOrOp_rhs_operand_i};
enum E_SgBitXorOp 
{
SgBitXorOp_lhs_operand_i, SgBitXorOp_rhs_operand_i};
enum E_SgBitAndOp 
{
SgBitAndOp_lhs_operand_i, SgBitAndOp_rhs_operand_i};
enum E_SgBitOrOp 
{
SgBitOrOp_lhs_operand_i, SgBitOrOp_rhs_operand_i};
enum E_SgBitEqvOp 
{
SgBitEqvOp_lhs_operand_i, SgBitEqvOp_rhs_operand_i};
enum E_SgCommaOpExp 
{
SgCommaOpExp_lhs_operand_i, SgCommaOpExp_rhs_operand_i};
enum E_SgLshiftOp 
{
SgLshiftOp_lhs_operand_i, SgLshiftOp_rhs_operand_i};
enum E_SgRshiftOp 
{
SgRshiftOp_lhs_operand_i, SgRshiftOp_rhs_operand_i};
enum E_SgPntrArrRefExp 
{
SgPntrArrRefExp_lhs_operand_i, SgPntrArrRefExp_rhs_operand_i};
enum E_SgScopeOp 
{
SgScopeOp_lhs_operand_i, SgScopeOp_rhs_operand_i};
enum E_SgAssignOp 
{
SgAssignOp_lhs_operand_i, SgAssignOp_rhs_operand_i};
enum E_SgExponentiationOp 
{
SgExponentiationOp_lhs_operand_i, SgExponentiationOp_rhs_operand_i};
enum E_SgJavaUnsignedRshiftOp 
{
SgJavaUnsignedRshiftOp_lhs_operand_i, SgJavaUnsignedRshiftOp_rhs_operand_i};
enum E_SgConcatenationOp 
{
SgConcatenationOp_lhs_operand_i, SgConcatenationOp_rhs_operand_i};
enum E_SgPointerAssignOp 
{
SgPointerAssignOp_lhs_operand_i, SgPointerAssignOp_rhs_operand_i};
enum E_SgUserDefinedBinaryOp 
{
SgUserDefinedBinaryOp_lhs_operand_i, SgUserDefinedBinaryOp_rhs_operand_i};
enum E_SgPlusAssignOp 
{
SgPlusAssignOp_lhs_operand_i, SgPlusAssignOp_rhs_operand_i};
enum E_SgMinusAssignOp 
{
SgMinusAssignOp_lhs_operand_i, SgMinusAssignOp_rhs_operand_i};
enum E_SgAndAssignOp 
{
SgAndAssignOp_lhs_operand_i, SgAndAssignOp_rhs_operand_i};
enum E_SgIorAssignOp 
{
SgIorAssignOp_lhs_operand_i, SgIorAssignOp_rhs_operand_i};
enum E_SgMultAssignOp 
{
SgMultAssignOp_lhs_operand_i, SgMultAssignOp_rhs_operand_i};
enum E_SgDivAssignOp 
{
SgDivAssignOp_lhs_operand_i, SgDivAssignOp_rhs_operand_i};
enum E_SgModAssignOp 
{
SgModAssignOp_lhs_operand_i, SgModAssignOp_rhs_operand_i};
enum E_SgXorAssignOp 
{
SgXorAssignOp_lhs_operand_i, SgXorAssignOp_rhs_operand_i};
enum E_SgLshiftAssignOp 
{
SgLshiftAssignOp_lhs_operand_i, SgLshiftAssignOp_rhs_operand_i};
enum E_SgRshiftAssignOp 
{
SgRshiftAssignOp_lhs_operand_i, SgRshiftAssignOp_rhs_operand_i};
enum E_SgJavaUnsignedRshiftAssignOp 
{
SgJavaUnsignedRshiftAssignOp_lhs_operand_i, SgJavaUnsignedRshiftAssignOp_rhs_operand_i};
enum E_SgIntegerDivideAssignOp 
{
SgIntegerDivideAssignOp_lhs_operand_i, SgIntegerDivideAssignOp_rhs_operand_i};
enum E_SgExponentiationAssignOp 
{
SgExponentiationAssignOp_lhs_operand_i, SgExponentiationAssignOp_rhs_operand_i};
enum E_SgCompoundAssignOp 
{
SgCompoundAssignOp_lhs_operand_i, SgCompoundAssignOp_rhs_operand_i};
enum E_SgMembershipOp 
{
SgMembershipOp_lhs_operand_i, SgMembershipOp_rhs_operand_i};
enum E_SgSpaceshipOp 
{
SgSpaceshipOp_lhs_operand_i, SgSpaceshipOp_rhs_operand_i};
enum E_SgNonMembershipOp 
{
SgNonMembershipOp_lhs_operand_i, SgNonMembershipOp_rhs_operand_i};
enum E_SgIsOp 
{
SgIsOp_lhs_operand_i, SgIsOp_rhs_operand_i};
enum E_SgIsNotOp 
{
SgIsNotOp_lhs_operand_i, SgIsNotOp_rhs_operand_i};
enum E_SgDotDotExp 
{
SgDotDotExp_lhs_operand_i, SgDotDotExp_rhs_operand_i};
enum E_SgElementwiseMultiplyOp 
{
SgElementwiseMultiplyOp_lhs_operand_i, SgElementwiseMultiplyOp_rhs_operand_i};
enum E_SgElementwisePowerOp 
{
SgElementwisePowerOp_lhs_operand_i, SgElementwisePowerOp_rhs_operand_i};
enum E_SgElementwiseLeftDivideOp 
{
SgElementwiseLeftDivideOp_lhs_operand_i, SgElementwiseLeftDivideOp_rhs_operand_i};
enum E_SgElementwiseDivideOp 
{
SgElementwiseDivideOp_lhs_operand_i, SgElementwiseDivideOp_rhs_operand_i};
enum E_SgElementwiseAddOp 
{
SgElementwiseAddOp_lhs_operand_i, SgElementwiseAddOp_rhs_operand_i};
enum E_SgElementwiseSubtractOp 
{
SgElementwiseSubtractOp_lhs_operand_i, SgElementwiseSubtractOp_rhs_operand_i};
enum E_SgElementwiseOp 
{
SgElementwiseOp_lhs_operand_i, SgElementwiseOp_rhs_operand_i};
enum E_SgPowerOp 
{
SgPowerOp_lhs_operand_i, SgPowerOp_rhs_operand_i};
enum E_SgLeftDivideOp 
{
SgLeftDivideOp_lhs_operand_i, SgLeftDivideOp_rhs_operand_i};
enum E_SgRemOp 
{
SgRemOp_lhs_operand_i, SgRemOp_rhs_operand_i};
enum E_SgReplicationOp 
{
SgReplicationOp_lhs_operand_i, SgReplicationOp_rhs_operand_i};
enum E_SgAtOp 
{
SgAtOp_lhs_operand_i, SgAtOp_rhs_operand_i};
enum E_SgBinaryOp 
{
SgBinaryOp_lhs_operand_i, SgBinaryOp_rhs_operand_i};
enum E_SgListExp 
{
SgListExp_expressions};
enum E_SgTupleExp 
{
SgTupleExp_expressions};
enum E_SgMatrixExp 
{
SgMatrixExp_expressions};
enum E_SgExprListExp 
{
SgExprListExp_expressions};
enum E_SgComplexVal 
{
SgComplexVal_real_value, SgComplexVal_imaginary_value};
enum E_SgCudaKernelCallExp 
{
SgCudaKernelCallExp_function, SgCudaKernelCallExp_args, SgCudaKernelCallExp_exec_config};
enum E_SgFunctionCallExp 
{
SgFunctionCallExp_function, SgFunctionCallExp_args};
enum E_SgCallExpression 
{
SgCallExpression_function, SgCallExpression_args};
enum E_SgSizeOfOp 
{
SgSizeOfOp_operand_expr};
enum E_SgUpcLocalsizeofExpression 
{
SgUpcLocalsizeofExpression_expression};
enum E_SgUpcBlocksizeofExpression 
{
SgUpcBlocksizeofExpression_expression};
enum E_SgUpcElemsizeofExpression 
{
SgUpcElemsizeofExpression_expression};
enum E_SgJavaInstanceOfOp 
{
SgJavaInstanceOfOp_operand_expr};
enum E_SgTypeIdOp 
{
SgTypeIdOp_operand_expr};
enum E_SgConditionalExp 
{
SgConditionalExp_conditional_exp, SgConditionalExp_true_exp, SgConditionalExp_false_exp};
enum E_SgNewExp 
{
SgNewExp_placement_args, SgNewExp_constructor_args, SgNewExp_builtin_args};
enum E_SgDeleteExp 
{
SgDeleteExp_variable};
enum E_SgAggregateInitializer 
{
SgAggregateInitializer_initializers};
enum E_SgCompoundInitializer 
{
SgCompoundInitializer_initializers};
enum E_SgConstructorInitializer 
{
SgConstructorInitializer_args};
enum E_SgAssignInitializer 
{
SgAssignInitializer_operand_i};
enum E_SgDesignatedInitializer 
{
SgDesignatedInitializer_designatorList, SgDesignatedInitializer_memberInit};
enum E_SgBracedInitializer 
{
SgBracedInitializer_initializers};
enum E_SgVarArgStartOp 
{
SgVarArgStartOp_lhs_operand, SgVarArgStartOp_rhs_operand};
enum E_SgVarArgOp 
{
SgVarArgOp_operand_expr};
enum E_SgVarArgEndOp 
{
SgVarArgEndOp_operand_expr};
enum E_SgVarArgCopyOp 
{
SgVarArgCopyOp_lhs_operand, SgVarArgCopyOp_rhs_operand};
enum E_SgVarArgStartOneOperandOp 
{
SgVarArgStartOneOperandOp_operand_expr};
enum E_SgSubscriptExpression 
{
SgSubscriptExpression_lowerBound, SgSubscriptExpression_upperBound, SgSubscriptExpression_stride};
enum E_SgImpliedDo 
{
SgImpliedDo_do_var_initialization, SgImpliedDo_last_val, SgImpliedDo_increment, SgImpliedDo_object_list};
enum E_SgIOItemExpression 
{
SgIOItemExpression_io_item};
enum E_SgStatementExpression 
{
SgStatementExpression_statement};
enum E_SgAsmOp 
{
SgAsmOp_expression};
enum E_SgActualArgumentExpression 
{
SgActualArgumentExpression_expression};
enum E_SgUnknownArrayOrFunctionReference 
{
SgUnknownArrayOrFunctionReference_named_reference, SgUnknownArrayOrFunctionReference_expression_list};
enum E_SgCAFCoExpression 
{
SgCAFCoExpression_referData};
enum E_SgCudaKernelExecConfig 
{
SgCudaKernelExecConfig_grid, SgCudaKernelExecConfig_blocks, SgCudaKernelExecConfig_shared, SgCudaKernelExecConfig_stream};
enum E_SgLambdaRefExp 
{
SgLambdaRefExp_functionDeclaration};
enum E_SgDictionaryExp 
{
SgDictionaryExp_key_datum_pairs};
enum E_SgKeyDatumPair 
{
SgKeyDatumPair_key, SgKeyDatumPair_datum};
enum E_SgComprehension 
{
SgComprehension_target, SgComprehension_iter, SgComprehension_filters};
enum E_SgListComprehension 
{
SgListComprehension_element, SgListComprehension_generators};
enum E_SgSetComprehension 
{
SgSetComprehension_element, SgSetComprehension_generators};
enum E_SgDictionaryComprehension 
{
SgDictionaryComprehension_element, SgDictionaryComprehension_generators};
enum E_SgNaryBooleanOp 
{
SgNaryBooleanOp_operands};
enum E_SgNaryComparisonOp 
{
SgNaryComparisonOp_operands};
enum E_SgNaryOp 
{
SgNaryOp_operands};
enum E_SgStringConversion 
{
SgStringConversion_expression};
enum E_SgYieldExpression 
{
SgYieldExpression_value};
enum E_SgAlignOfOp 
{
SgAlignOfOp_operand_expr};
enum E_SgRangeExp 
{
SgRangeExp_start, SgRangeExp_end, SgRangeExp_stride};
enum E_SgJavaSingleMemberAnnotation 
{
SgJavaSingleMemberAnnotation_value};
enum E_SgJavaNormalAnnotation 
{
SgJavaNormalAnnotation_value_pair_list};
enum E_SgFunctionParameterRefExp 
{
SgFunctionParameterRefExp_parameter_expression};
enum E_SgLambdaExp 
{
SgLambdaExp_lambda_capture_list, SgLambdaExp_lambda_closure_class, SgLambdaExp_lambda_function};
enum E_SgHereExp 
{
SgHereExp_expression};
enum E_SgAtExp 
{
SgAtExp_expression, SgAtExp_body};
enum E_SgFinishExp 
{
SgFinishExp_expression, SgFinishExp_body};
enum E_SgNoexceptOp 
{
SgNoexceptOp_operand_expr};
enum E_SgFoldExpression 
{
SgFoldExpression_operands};
enum E_SgAwaitExpression 
{
SgAwaitExpression_value};
enum E_SgChooseExpression 
{
SgChooseExpression_value};
enum E_SgJovialTablePresetExp 
{
SgJovialTablePresetExp_default_sublist, SgJovialTablePresetExp_specified_sublist};
enum E_SgNonrealSymbol 
{
SgNonrealSymbol_declaration};
enum E_SgTemplateMemberFunctionSymbol 
{
SgTemplateMemberFunctionSymbol_declaration};
enum E_SgMemberFunctionSymbol 
{
SgMemberFunctionSymbol_declaration};
enum E_SgTemplateFunctionSymbol 
{
SgTemplateFunctionSymbol_declaration};
enum E_SgRenameSymbol 
{
SgRenameSymbol_declaration, SgRenameSymbol_original_symbol};
enum E_SgFunctionSymbol 
{
SgFunctionSymbol_declaration};
enum E_SgTemplateClassSymbol 
{
SgTemplateClassSymbol_declaration};
enum E_SgClassSymbol 
{
SgClassSymbol_declaration};
enum E_SgTemplateSymbol 
{
SgTemplateSymbol_declaration};
enum E_SgEnumSymbol 
{
SgEnumSymbol_declaration};
enum E_SgTemplateTypedefSymbol 
{
SgTemplateTypedefSymbol_declaration};
enum E_SgTypedefSymbol 
{
SgTypedefSymbol_declaration};
enum E_SgLabelSymbol 
{
SgLabelSymbol_declaration};
enum E_SgNamespaceSymbol 
{
SgNamespaceSymbol_declaration, SgNamespaceSymbol_aliasDeclaration};
enum E_SgModuleSymbol 
{
SgModuleSymbol_declaration};
enum E_SgAliasSymbol 
{
SgAliasSymbol_alias};
enum E_SgJavaLabelSymbol 
{
SgJavaLabelSymbol_declaration};
enum E_SgAdaPackageSymbol 
{
SgAdaPackageSymbol_declaration};
enum E_SgAdaTaskSymbol 
{
SgAdaTaskSymbol_declaration};

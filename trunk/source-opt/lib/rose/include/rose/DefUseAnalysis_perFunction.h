/******************************************
 * Category: DFA
 * DefUse Analysis Declaration (per Function)
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DefUseAnalysisPF_HXX_LOADED__
#define __DefUseAnalysisPF_HXX_LOADED__
#include <string>
#include <set>

#include "DefUseAnalysisAbstract.h"

class DefUseAnalysisPF : public DefUseAnalysisAbstract {
 private:


  bool makeSureThatTheUseIsInTable(SgInitializedName* initName);
  bool makeSureThatTheDefIsInTable(SgInitializedName* initName);

  template <typename T>
    bool hasANodeAboveCurrentChanged(T cfgNode, std::set<SgNode*> &visited);

  template <typename T>
    bool hasANodeIntheScopeChanged(T cfgNode);

  template <typename T> 
    bool performUseAndDefinition(SgNode* sgNode, SgInitializedName* initName,
                                 bool isUsage, bool isDefinition, 
                                 SgNode* sgNodeBefore, bool dont_replace,
                                 T cfgNode);


  // def-use-sepcific ------------------
  bool handleDefCopy(SgNode* sgNode, int nrOfInEdges, SgNode* sgNodeBefore, 
                     filteredCFGNodeType cfgNode);
  bool handleUseCopy(SgNode* sgNode, int nrOfInEdges, SgNode* sgNodeBefore, 
                     filteredCFGNodeType cfgNode);
  template <typename T> bool defuse(T cfgNode, bool *unhandled);

  int nrOfNodesVisitedPF;
  int breakPointForWhile;
  std::set<SgNode*>  breakPointForWhileNode;
  
 public:
  DefUseAnalysisPF(bool debug, DefUseAnalysis* dfa_p){
    DEBUG_MODE=debug;
    DEBUG_MODE_EXTRA=false;
    dfa=dfa_p;
    breakPointForWhile=0;
    //breakPointForWhileNode=NULL;
    breakPointForWhileNode.clear();
  };
  virtual ~DefUseAnalysisPF(){};
  FilteredCFGNode < IsDFAFilter > run(SgFunctionDefinition* function, bool& abortme);
  int getNumberOfNodesVisited();

};

#endif


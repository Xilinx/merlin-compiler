/******************************************
 * Category: DFA
 * DefUse Analysis Declaration
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DefUseAnalysis_HXX_LOADED__
#define __DefUseAnalysis_HXX_LOADED__
#include <string>

#include "filteredCFG.h"
#include "DFAnalysis.h"
#include "support.h"
#include "DFAFilter.h"

#include <iostream>

// CH (4/9/2010): Use boost::unordered instead
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#if 0
#ifdef _MSC_VER
#include <hash_map>
#include <hash_set>
#if 0
template <> struct hash <SgNode*> {
    size_t operator()(SgNode* const & n) const {
      return (size_t) n;
    }
    size_t operator()(SgNode* const & n1, SgNode* const & n2) const {
      ROSE_ASSERT(n1);
      ROSE_ASSERT(n2);
      return ( n1==n2);
    }
  };
#endif
#else
#include <ext/hash_map>
#include <ext/hash_set>

namespace rose_hash {
  template <> struct hash <SgNode*> {
    size_t operator()(SgNode* const & n) const {
      return (size_t) n;
    }
    size_t operator()(SgNode* const & n1, SgNode* const & n2) const {
      ROSE_ASSERT(n1);
      ROSE_ASSERT(n2);
      return ( n1==n2);
    }
  };
}
#endif

#endif 


class ROSE_DLL_API DefUseAnalysis : public DFAnalysis, Support {
 private:
  SgProject* project;
  bool DEBUG_MODE;
  bool DEBUG_MODE_EXTRA;
  std::vector<SgInitializedName*> globalVarList;

  bool visualizationEnabled;

  // def-use-specific --------------------
  typedef std::vector < std::pair<SgInitializedName* , SgNode*> > multitype;
  //typedef std::map < SgInitializedName* , std::set<SgNode*> > multitype;

  typedef std::map< SgNode* , multitype > tabletype;


  // typedef std::map< SgNode* , int > convtype;
// CH (4/9/2010): Use boost::unordered instead  
//#ifdef _MSC_VER
#if 0
  typedef hash_map< SgNode* , int > convtype;
#else
  //typedef __gnu_cxx::hash_map< SgNode* , int > convtype;
  typedef rose_hash::unordered_map< SgNode* , int > convtype;
#endif

  // local functions ---------------------
  void find_all_global_variables();
  bool start_traversal_of_functions();
  bool searchMap(const tabletype* ltable, SgNode* node);
  bool searchVizzMap(SgNode* node);
  std::string getInitName(SgNode* sgNode);

  // the main table of all entries
  tabletype table;   // definition table
  tabletype usetable;  // use table

  // the map to merge duplicated nodes
  std::map< SgNode*, SgNode*> same_def_map; // child -> root
  std::map< SgNode*, SgNode*> same_use_map; // child -> root
  std::map< SgNode*, std::set<SgNode*> > reverse_same_def_map; // root->children
  std::map< SgNode*, std::set<SgNode*> > reverse_same_use_map; // root->children
  std::map< SgNode*, bool> def_changed;
  std::map< SgNode*, bool> use_changed;
  typedef rose_hash::unordered_map<std::pair<SgNode*, SgScopeStatement*>, bool> isinscopetype;
  isinscopetype isinscope_cache;
  // table for indirect definitions
  //ideftype idefTable;
  // the helper table for visualization
  convtype vizzhelp;
  static int sgNodeCounter ;
  int nrOfNodesVisited;

  // functions to be printed in DFAtoDOT
  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;

  void addAnyElement(tabletype* tabl, SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void mapAnyUnion(tabletype* tabl, SgNode* before, SgNode* other, SgNode* current); // current = before Union other
  void mapAnyUnion(tabletype* tabl, std::vector<SgNode*> &all_before_nodes, SgNode* current);
  void printAnyMap(tabletype* tabl, const std::map<SgNode*, std::set<SgNode*> > &);

  bool isInScope(SgNode* node, SgScopeStatement* scope);

 public:
  DefUseAnalysis(SgProject* proj, bool debug = false): project(proj), 
    DEBUG_MODE(debug), DEBUG_MODE_EXTRA(false){
    find_all_global_variables();
    //visualizationEnabled=true;
    //table.clear();
    //usetable.clear();
    //globalVarList.clear();
    //vizzhelp.clear();
    //sgNodeCounter=0;
  };
  virtual ~DefUseAnalysis() {}

  std::map< SgNode* , multitype  > getDefMap() { return table;}
  std::map< SgNode* , multitype  > getUseMap() { return usetable;}
  void setMaps(std::map< SgNode* , multitype  > def,
          std::map< SgNode* , multitype > use) {
    table = def;
    usetable = use;
  }
       
  // def-use-public-functions -----------
  int run();
  int run(bool debug);
  multitype getDefMultiMapFor(SgNode* node);
  multitype  getUseMultiMapFor(SgNode* node);
  std::vector < SgNode* > getAnyFor(const multitype* mul, SgInitializedName* initName);
  // for any given node, return all definitions of initName
  std::vector < SgNode* > getDefFor(SgNode* node, SgInitializedName* initName);
  std::vector < SgNode* > getUseFor(SgNode* node, SgInitializedName* initName);
  bool isNodeGlobalVariable(SgInitializedName* node);
  std::vector <SgInitializedName*> getGlobalVariables();
  // the following one is used for parallel traversal
  int start_traversal_of_one_function(SgFunctionDefinition* proc);

  // helpers -----------------------------
  bool searchMap(SgNode* node);
  int getDefSize();
  int getUseSize();
  void printMultiMap(const multitype* type);
  void printMultiMap(const multitype& type);
  void printDefMap();  
  void printUseMap();

  bool addID(SgNode* sgNode);
  void addDefElement(SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void addUseElement(SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void replaceElement(SgNode* sgNode, SgInitializedName* initName);
  void mapDefUnion(SgNode* before, SgNode* other, SgNode* current);
  void mapUseUnion(SgNode* before, SgNode* other, SgNode* current);
  void mapDefUnion(const std::vector<SgNode*> &all_before_nodes, SgNode* current);
  void mapUseUnion(const std::vector<SgNode*> &all_before_nodes, SgNode* current);

  void clearUseOfElement(SgNode* sgNode, SgInitializedName* initName);

  int getIntForSgNode(SgNode* node);
  void dfaToDOT();
  //void addIDefElement(SgNode* sgNode, SgInitializedName* initName);

  // clear the tables if necessary
  void flush() {
   table.clear();
   usetable.clear();
   globalVarList.clear();
   vizzhelp.clear();
   sgNodeCounter=1;
   //  nrOfNodesVisited=0;
  }

  void flushHelp() {
   vizzhelp.clear();
   sgNodeCounter=1;
  }

  void disableVisualization() {
    visualizationEnabled=false;
  }

  void flushDefuse() {
   table.clear();
   usetable.clear();
   //   vizzhelp.clear();
   //sgNodeCounter=1;
  }
  
  void notifyChildrenChanged(SgNode* node, std::map<SgNode*, bool> &changed_map); 
  void updateRootStatus(SgNode* node, bool changed); 
  bool checkRootChanged(SgNode* node, const std::map<SgNode*, bool>& changed_map);

  bool isUseChanged(SgNode *node, bool &valid);
  bool isDefChanged(SgNode *node, bool &valid);
};

#endif

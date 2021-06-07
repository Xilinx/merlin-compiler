// Declarations and templates for supporting boost::serialization of Sage IR nodes.
#ifndef ROSE_Cxx_GrammarSerialization_H
#define ROSE_Cxx_GrammarSerialization_H

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB

// sage3basic.h or rose.h must be inlucded first from a .C file (don't do it here!)
#include <boost/serialization/export.hpp>

// The declaration half of exporting polymorphic classes.
BOOST_CLASS_EXPORT_KEY(SgFile);
BOOST_CLASS_EXPORT_KEY(SgSupport);


/** Register all Sage IR node types for serialization.
 *
 *  This function should be called before any Sage IR nodes are serialized or deserialized. It
 *  registers all SgNode subclasses that might be serialized through a base pointer. Note that
 *  registration is required but not sufficient for serialization: the "serialize" member
 *  function template must also be defined. */
template<class Archive>
void roseAstSerializationRegistration(Archive &archive) {
    archive.template register_type<SgFile>();
    archive.template register_type<SgSupport>();
}

#endif
#endif

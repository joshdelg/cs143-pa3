#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include <list>
#include <unordered_map>

#define TRUE 1
#define FALSE 0

class InheritanceNode;
typedef InheritanceNode *InheritanceNodeP;
class ClassTable;
typedef ClassTable *ClassTableP;

struct TypeInfo {
  Symbol type; /* The type of the ObjectId */
  object_class *object; /* The object itself */
};

typedef SymbolTable<Symbol, TypeInfo> Environment;
typedef Environment *EnvironmentP;

class InheritanceNode {
public:
    Class_ class_node;
    Symbol name;
    
    InheritanceNodeP parent;

    // Track the methods and attributes of the class, since these don't have to be
    // declared before use
    std::unordered_map<Symbol, FeatureOverrideInfoP> methods;
    std::unordered_map<Symbol, FeatureOverrideInfoP> attributes;
};


// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.
class ClassTable : public SymbolTable<Symbol, InheritanceNode> {
private:
  int semant_errors;           // counts the number of semantic errors
  void install_basic_classes();
  std::ostream& error_stream;

  void print_debug_hierarchy();

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }

  /** Call only when errors() is still 0 after construction (valid inheritance graph). */
  void collect_methods_and_attributes(Classes classes);
  void check_main_class_and_method();

  std::ostream& semant_error();
  std::ostream& semant_error(Class_ c);
  std::ostream& semant_error(Symbol filename, tree_node *t);

  /**
   * Lookup a method or attribute by its symbol name for purposes of typechecking.
   * Both are declared as Feature, calling ->get_type() on the result will use the
   * correct field to access type through runtime dispatch.

   * Returns the bottommost overload found on the inheritance hierarchy
  */
  Feature lookup_method(Symbol name, Symbol class_name, Symbol current_class);
  Feature lookup_attribute(Symbol name, Symbol class_name, Symbol current_class);
  /** Resolve `SELF_TYPE` as class `current_class`, then `lookup`. */
  InheritanceNode *lookup_in_context(Symbol type_name, Symbol current_class);

  bool is_equal_class(Symbol a, Symbol b);
  bool is_subclass(Symbol a, Symbol b);
  bool is_subclass_given_context(Symbol a, Symbol b, Symbol C);
  Symbol class_join(Symbol a, Symbol b);
};


#endif

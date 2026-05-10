#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include <list>

#define TRUE 1
#define FALSE 0

class InheritanceNode;
typedef InheritanceNode *InheritanceNodeP;
class ClassTable;
typedef ClassTable *ClassTableP;

class InheritanceNode {
public:
    Class_ class_node;
    Symbol name;
    
    InheritanceNodeP parent;

    // Track the methods and attributes of the class, since these don't have to be
    // declared before use
    std::list<Symbol> methods;
    std::list<Symbol> attributes;
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

  /**
   * Collects all the methods and attributes of the classes in the program
   * and adds them to the symbol table. Includes methods and attributes inherited from parent classes.
   */
  void collect_methods_and_attributes();

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  std::ostream& semant_error();
  std::ostream& semant_error(Class_ c);
  std::ostream& semant_error(Symbol filename, tree_node *t);

  bool is_equal_class(Symbol a, Symbol b);
  bool is_subclass(Symbol a, Symbol b);
  Symbol class_join(Symbol a, Symbol b);
};


#endif

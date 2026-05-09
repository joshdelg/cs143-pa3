

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"

extern int semant_debug;
extern char *curr_filename;
extern int node_lineno;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol
       arg,
       arg2,
       Bool,
       concat,
       cool_abort,
       copy,
       Int,
       in_int,
       in_string,
       IO,
       isProto,
       length,
       Main,
       main_meth,
       No_class,
       No_type,
       _BOTTOM_,
       Object,
       out_int,
       out_string,
       prim_slot,
       self,
       SELF_TYPE,
       Str,
       str_field,
       substr,
       type_name,
       val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void) {
  arg         = idtable.add_string("arg");
  arg2        = idtable.add_string("arg2");
  Bool        = idtable.add_string("Bool");
  concat      = idtable.add_string("concat");
  cool_abort  = idtable.add_string("abort");
  ::copy      = idtable.add_string("copy");
  Int         = idtable.add_string("Int");
  in_int      = idtable.add_string("in_int");
  in_string   = idtable.add_string("in_string");
  IO          = idtable.add_string("IO");
  isProto     = idtable.add_string("isProto");
  length      = idtable.add_string("length");
  Main        = idtable.add_string("Main");
  main_meth   = idtable.add_string("main");
  //   _no_class is a symbol that can't be the name of any
  //   user-defined class.
  No_class    = idtable.add_string("_no_class");
  No_type     = idtable.add_string("_no_type");
  // _BOTTOM_ is the symbol for the bottom of the lattice of types
  _BOTTOM_    = idtable.add_string("_bottom");
  Object      = idtable.add_string("Object");
  out_int     = idtable.add_string("out_int");
  out_string  = idtable.add_string("out_string");
  prim_slot   = idtable.add_string("_prim_slot");
  self        = idtable.add_string("self");
  SELF_TYPE   = idtable.add_string("SELF_TYPE");
  Str         = idtable.add_string("String");
  str_field   = idtable.add_string("_str_field");
  substr      = idtable.add_string("substr");
  type_name   = idtable.add_string("type_name");
  val         = idtable.add_string("_val");
}

ClassTable::ClassTable(Classes classes) : semant_errors(0), error_stream(cerr) {
    enterscope(); // global scope, we never actually use other scopes for the ClassTable

    // Must call after enterscope because cannot add classes to the table without a scope
    install_basic_classes();
    /* Build flat SymbolTable of our classes, essentially a mapping from class
       name to orphan InheritanceNodes.
    */
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ c = classes->nth(i);
        Symbol class_name = c->get_name();

        if (class_name == SELF_TYPE) {
            // error
            return;
        }

        /* Build InheritanceNode */
        InheritanceNodeP new_node = new InheritanceNode();
        new_node->class_node = c;
        new_node->name = class_name;
        new_node->parent = NULL;

        /* Check for duplicates */
        if (probe(class_name) != NULL) {
            // error
            return;
        }

        /* Add to SymbolTable */
        addid(class_name, new_node);
    }

    /* Link our InheritanceNodes together. */
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ c = classes->nth(i);
        Symbol parent_name = c->get_parent();

        InheritanceNodeP child_node = lookup(c->get_name());
        InheritanceNodeP parent_node = lookup(parent_name);

        child_node->parent = parent_node;
    }

    /* Check for cycles */
    int num_classes = classes->len();
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ c = classes->nth(i);
        InheritanceNodeP current_node = lookup(c->get_name());

        /* If we ever end up traversing more nodes than there are classes, 
           we're in a cycle of some sort.
        */
        int moves = 0;
        while (current_node != NULL) {
            current_node = current_node->parent;
            moves++;
            if (moves > num_classes) {
                // error
                return;
            }
        }
    }

    print_debug_hierarchy();

    /* ClassTable should be complete */
    return;
}

void ClassTable::install_basic_classes() {
  // The tree package uses these globals to annotate the classes built below.
  node_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

  // The following demonstrates how to create dummy parse trees to
  // refer to basic Cool classes.  There's no need for method
  // bodies -- these are already built into the runtime system.

  // IMPORTANT: The results of the following expressions are
  // stored in local variables.  You will want to do something
  // with those variables at the end of this method to make this
  // code meaningful.


  //
  // The Object class has no parent class. Its methods are
  //        cool_abort() : Object    aborts the program
  //        type_name() : Str        returns a string representation of class name
  //        copy() : SELF_TYPE       returns a copy of the object
  //
  // There is no need for method bodies in the basic classes---these
  // are already built in to the runtime system.
  //

  Class_ Object_class =
    class_(Object,
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(::copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename);

  //
  // The IO class inherits from Object. Its methods are
  //        out_string(Str) : SELF_TYPE          writes a string to the output
  //        out_int(Int) : SELF_TYPE               "    an int    "  "     "
  //        in_string() : Str                    reads a string from the input
  //        in_int() : Int                         "   an int     "  "     "
  //

  Class_ IO_class =
     class_(IO,
            Object,
            append_Features(
            append_Features(
            append_Features(
            single_Features(method(out_string, single_Formals(formal(arg, Str)),
                        SELF_TYPE, no_expr())),
            single_Features(method(out_int, single_Formals(formal(arg, Int)),
                        SELF_TYPE, no_expr()))),
            single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
            single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	    filename);

  //
  // The Int class has no methods and only a single attribute, the
  // "val" for the integer.
  //

  Class_ Int_class =
      class_(Int,
	     Object,
	     single_Features(attr(val, prim_slot, no_expr())),
	     filename);

  //
  // Bool also has only the "val" slot.
  //

  Class_ Bool_class =
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

  //
  // The class Str has a number of slots and operations:
  //       val                                  the length of the string
  //       str_field                            the string itself
  //       length() : Int                       returns length of the string
  //       concat(arg: Str) : Str               performs string concatenation
  //       substr(arg: Int, arg2: Int): Str     substring selection
  //

  Class_ Str_class =
      class_(Str,
	     Object,
             append_Features(
             append_Features(
             append_Features(
             append_Features(
             single_Features(attr(val, Int, no_expr())),
            single_Features(attr(str_field, prim_slot, no_expr()))),
            single_Features(method(length, nil_Formals(), Int, no_expr()))),
            single_Features(method(concat,
				   single_Formals(formal(arg, Str)),
				   Str,
				   no_expr()))),
	    single_Features(method(substr,
				   append_Formals(single_Formals(formal(arg, Int)),
						  single_Formals(formal(arg2, Int))),
				   Str,
				   no_expr()))),
	     filename);



    /* Add basic classes to ClassTable */
    InheritanceNodeP object_node = new InheritanceNode();
    object_node->class_node = Object_class;
    object_node->name = Object;
    object_node->parent = NULL;
    addid(Object, object_node);

    InheritanceNodeP io_node = new InheritanceNode();
    io_node->class_node = IO_class;
    io_node->name = IO;
    io_node->parent = object_node;
    addid(IO, io_node);

    InheritanceNodeP int_node = new InheritanceNode();
    int_node->class_node = Int_class;
    int_node->name = Int;
    int_node->parent = object_node;
    addid(Int, int_node);

    InheritanceNodeP bool_node = new InheritanceNode();
    bool_node->class_node = Bool_class;
    bool_node->name = Bool;
    bool_node->parent = object_node;
    addid(Bool, bool_node);

    InheritanceNodeP str_node = new InheritanceNode();
    str_node->class_node = Str_class;
    str_node->name = Str;
    str_node->parent = object_node;
    addid(Str, str_node);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
//       print a line number and filename
//       (line number is extracted from tree_node)
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{
  return semant_error(c->get_filename(),c);
}

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
  error_stream << filename << ":" << t->get_line_number() << ": ";
  return semant_error();
}

ostream& ClassTable::semant_error()
{
    semant_errors++;
    return error_stream;
}

void ClassTable::print_debug_hierarchy() {
    cout << "Printing debug hierarchy" << endl;
    // For each node, print the full parent lineage
    // Iterate through the nodes in the ClassTable by getting the first scope and iterating through members
    ScopeList& scope_list = gettable();
    if (scope_list.empty()) {
        cout << "No scopes in ClassTable" << endl;
        return;
    }

    Scope first_scope = scope_list.front();
    for (auto entry = first_scope.begin(); entry != first_scope.end(); entry++) {
        InheritanceNodeP node = entry->get_info();

        cout << node->name << " -> ";

        while (node->parent != NULL) {
            cout << node->parent->name << " -> ";
            node = node->parent;
        }
        cout << endl;
    }
}

bool ClassTable::is_equal_class(Symbol a, Symbol b)
{
    return a == b;
}

bool ClassTable::is_subclass(Symbol child, Symbol ancestor)
{
    InheritanceNode *current_node = lookup(child);
    InheritanceNode *ancestor_node = lookup(ancestor);

    while (current_node != NULL) {
        if (current_node->class_node == ancestor_node->class_node) {
            return true;
        }
        current_node = current_node->parent;
    }

    return false;
}

Symbol ClassTable::class_join(Symbol a, Symbol b) 
{
    InheritanceNode *node_a = lookup(a);
    InheritanceNode *node_b = lookup(b);

    while (node_a != NULL) {
        InheritanceNode *current_node_b = node_b;
        while (current_node_b != NULL) {
            if (node_a->class_node == current_node_b->class_node) {
                return node_a->name;
            }
            current_node_b = current_node_b->parent;
        }
        node_a = node_a->parent;
    }

    return NULL;
}

/*
 * This is the entry point to the semantic checker.
 *
 * Your checker should do the following two things:
 *
 *   1) Check that the program is semantically correct
 *   2) Decorate the abstract syntax tree with type information
 *      by setting the `type' field in each Expression node.
 *      (see `tree.h')
 *
 *   You are free to first do 1), make sure you catch all semantic
 *   errors. Part 2) can be done in a second stage, when you want
 *   to build mycoolc.
 */
void program_class::semant() {
   initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
   ClassTableP classtable = new ClassTable(classes);

   if (classtable->errors()) {
      cerr << "Compilation halted due to static semantic errors." << endl;
      exit(1);
   }
}

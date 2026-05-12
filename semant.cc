

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>
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
            semant_error(c) << "Class cannot be named SELF_TYPE.\n";
            return;
        }

        /* Build InheritanceNode */
        InheritanceNodeP new_node = new InheritanceNode();
        new_node->class_node = c;
        new_node->name = class_name;
        new_node->parent = NULL;

        /* Check for duplicates */
        if (probe(class_name) != NULL) {
            semant_error(c) << "Class " << class_name << " is already defined.\n";
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

        if (parent_name == Int || parent_name == Bool || parent_name == Str) {
            semant_error(c) << "Class " << c->get_name() << " cannot inherit from basic class " << parent_name << ".\n";
            return;
        }

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
                semant_error(c) << "Inheritance cycle detected for class " << c->get_name() << ".\n";
                return;
            }
        }
    }

    collect_methods_and_attributes();

    /* Ensure there is a Main class and a main method */
    InheritanceNode *main_node = lookup(Main);
    if (main_node == NULL) {
        semant_error() << "Class Main is not defined.\n";
    } else {
        Feature main_method = lookup_method(main_meth, Main);
        if (main_method == NULL) {
            semant_error(main_node->class_node) << "No 'main' method defined in class Main.\n";
        } else if (main_method->get_formals()->len() != 0) {
            semant_error(main_node->class_node) << "'main' method in class Main should have no arguments.\n";
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

void ClassTable::collect_methods_and_attributes() {
    // Iterate through each class
    ScopeList& scope_list = gettable();
    if (scope_list.empty()) {
        cout << "No scopes in ClassTable" << endl;
        return;
    }

    // We keep a set of classes we've already set up so we don't touch them.
    std::unordered_set<Symbol> initialized;

    Scope first_scope = scope_list.front();
    for (auto entry = first_scope.begin(); entry != first_scope.end(); entry++) {
        InheritanceNodeP node = entry->get_info();
        if (initialized.count(node->name)) continue;


        // We want to traverse from top->down so we can do overriding correctly.
        std::vector<InheritanceNodeP> chain;
        InheritanceNodeP current = node;
        while (current != NULL) {
            chain.push_back(current);
            current = current->parent;
        }
        std::reverse(chain.begin(), chain.end());

        // Traverse down from Object to this class, accumulating methods and attributes as we go down.
        for (InheritanceNodeP ancestor : chain) {
            if (initialized.count(ancestor->name)) continue;

            // Inherit parent's items, already initialized because we're moving down the chain.
            // (Object has no parent)
            if (ancestor->parent != NULL) {
                ancestor->methods = ancestor->parent->methods;
                ancestor->attributes = ancestor->parent->attributes;
            }

            // Add this class's own features.
            Features features = ancestor->class_node->get_features();
            for (int i = features->first(); features->more(i); i = features->next(i)) {
                Feature feature = features->nth(i);

                std::string err;
                if (!feature->register_method_or_attribute(ancestor->methods, ancestor->attributes, err,
                                                          ancestor->class_node)) {
                    semant_error(ancestor->class_node->get_filename(), feature) << err << endl;
                }
            }

            initialized.insert(ancestor->name);
        }
    }
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

        node = entry->get_info();
        for (auto& m : node->methods)
            cout << "-method:" << m.first << ":" << m.second->feature->get_type() << endl;
        for (auto& a : node->attributes)
            cout << "-attr:" << a.first << ":" << a.second->feature->get_type() << endl;
    }
}

Feature ClassTable::lookup_method(Symbol name, Symbol class_name) {
    // NULL if class not found
    InheritanceNode *node = lookup(class_name);
    if (node == NULL) return NULL;

    // ... or if method not found (.find allows a safe lookup)
    auto iter = node->methods.find(name);
    if (iter == node->methods.end()) return NULL;

    // iter entry -> FeatureOverrideInfo -> feature
    return iter->second->feature;

}


Feature ClassTable::lookup_attribute(Symbol name, Symbol class_name) {
    // NULL if class not found
    InheritanceNode *node = lookup(class_name); 
    if (node == NULL) return NULL;

    // ... or if attribute not found (.find allows a safe lookup)
    auto it = node->attributes.find(name);
    if (it == node->attributes.end()) return NULL;

    // it entry -> FeatureOverrideInfo -> feature
    return it->second->feature;
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

// This subclass function does a quick check for SELF_TYPE and redirects to the normal is_subclass function if applicable.
bool ClassTable::is_subclass_given_context(Symbol a, Symbol b, Symbol C) {
    Symbol actual_a;
    Symbol actual_b;

    if (a == SELF_TYPE) {
        actual_a = C;
    } else {
        actual_a = a;
    }

    if (b == SELF_TYPE) {
        actual_b = C;
    } else {
        actual_b = b;
    }

    return is_subclass(actual_a, actual_b);
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


/*** AST Method Implementations */

bool method_class::register_method_or_attribute(std::unordered_map<Symbol, FeatureOverrideInfoP> &methods,
                                                std::unordered_map<Symbol, FeatureOverrideInfoP> &attributes,
                                                std::string &error_message, Class_ current_class) {
    (void)attributes;
    const char *n = name->get_string();
    FeatureOverrideInfoP existing = methods[name];

    if (existing != NULL) {
        Feature parent_m = existing->feature;
        if (get_formals()->len() != parent_m->get_formals()->len()) {
            error_message =
                std::string("Incompatible number of formal parameters in redefined method ") + n + ".";
            return false;
        }
        if (get_type() != parent_m->get_type()) {
            error_message = std::string("In redefined method ") + n + ", return type " +
                            std::string(get_type()->get_string()) + " is different from original return type " +
                            std::string(parent_m->get_type()->get_string()) + ".";
            return false;
        }
        for (int i = get_formals()->first(); get_formals()->more(i); i = get_formals()->next(i)) {
            Formal cf = get_formals()->nth(i);
            Formal pf = parent_m->get_formals()->nth(i);
            if (cf->get_type() != pf->get_type()) {
                error_message = std::string("In redefined method ") + n + ", parameter type " +
                                std::string(cf->get_type()->get_string()) +
                                " is different from original type " + std::string(pf->get_type()->get_string());
                return false;
            }
        }
    }

    methods[name] = new FeatureOverrideInfo(this, existing, current_class);
    return true;
}

bool attr_class::register_method_or_attribute(std::unordered_map<Symbol, FeatureOverrideInfoP> &methods,
                                              std::unordered_map<Symbol, FeatureOverrideInfoP> &attributes,
                                              std::string &error_message, Class_ current_class) {
    (void)methods;
    FeatureOverrideInfoP existing = attributes[name];
    if (existing != NULL) {
        if (existing->owner_class == current_class) {
            error_message =
                std::string("Attribute ") + name->get_string() + " is multiply defined in class.";
        } else {
            error_message =
                std::string("Attribute ") + name->get_string() + " is an attribute of an inherited class.";
        }
        return false;
    }

    attributes[name] = new FeatureOverrideInfo(this, nullptr, current_class);
    return true;
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

   /* Now, begin resolving names/scope and typechecking */
   Environment *env = new Environment();
   typecheck(classtable, env);
}


/* This is where all the typechecking functions will go.
Overall idea:
1) Recurse through the AST. Environment is a symbol table that tracks what's in scope
2) When you hit something that should open a new scope (class, method, attribute, block, let, cases, maybe I'm missing something), enter a new scope. When you exit one, exit the scope
    2a) In particular, when you enter a class, in the newly opened scope, add all the methods and attributes of the class to the environment.
3) When you hit one of the 4 things that introduces a new ObjectId (the spec says what), do an addid() on the environemnt symbol table
4) When you hit an objectId in an expression body, use the Environment's lookup function to find its type
5) When you hit a dispatch, lookup the method name using the ClassTable's lookup_method function on the dispatch'd class' name.

We need to implement the typecheck function for each node type. Many will just recurse and apply a typechecking rule from spec. The cases above need to do more.
Make sure to do pre-order/depth-first traversal so we can type subexpressions.
*/

void program_class::typecheck(ClassTable *class_table, Environment *env) {
    // So essentially we'll be building a shit ton of if statements that check all possible paths or type checking, save that type if it matches, and error if none do?
    // We'll be managing a symbol table simultaneously and use this as our primary tool when traversing the list.
    env->enterscope();
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ c = classes->nth(i);
        c->typecheck(class_table, env);
    }
    env->exitscope();
}

void class__class::typecheck(ClassTable *class_table, Environment *env) {
    env->enterscope();

    // From manual page 21 (The only unlabeled rule):
    // O_C (x) = {  SELF_TYPE_C   if T = SELF_TYPE
    //              T             otherwise

    // SELF_TYPE
    TypeInfo *self_info = new TypeInfo();
    self_info->type = SELF_TYPE;
    self_info->object = NULL;
    env->addid(self, self_info);

    // All other attributes (T's)
    InheritanceNode *node = class_table->lookup(name);
    for (auto& entry : node->attributes) {
        TypeInfo *info = new TypeInfo();
        info->type = entry.second->feature->get_type();
        info->object = NULL;
        env->addid(entry.first, info);
    }

    // Continue recursion
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        features->nth(i)->typecheck(class_table, env, name);
    }

    env->exitscope();
}


void method_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    Symbol filename = class_table->lookup(current_class)->class_node->get_filename();
    env->enterscope();

    // [Method]
    //
    // M(C,f) = (T1, ..., T_n, T_0)
    // O_C[SELF_TYPE_C / self][T_1/x_1]...[T_n/x_n], M, C |- e : T_0'
    // T_0' <= {  SELF_TYPE_C   if T_0 = SELF_TYPE
    //            T_0           otherwise
    //------------------------------------------------------------------
    // O_C , M, C |- f(x1 : T1, ..., xn : Tn) : T_0 { e }

    // Bind every formal parameters to its declared type for the body of the method.
    // (The O_C[SELF_TYPE_C / self][T_1/x_1]...[T_n/x_n] part)
    std::unordered_set<Symbol> seen_formals; // Track duplicates
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        Formal cur_formal = formals->nth(i);
        Symbol cur_formal_name = cur_formal->get_name();
        Symbol cur_formal_type = cur_formal->get_type();

        if (cur_formal_name == self) {
            class_table->semant_error(filename, cur_formal) << "'self' cannot be a formal parameter.\n";
            continue;
        }
        if (cur_formal_type == SELF_TYPE) {
            class_table->semant_error(filename, cur_formal) << "Formal parameter " << cur_formal_name << " cannot have type SELF_TYPE.\n";
        }
        if (seen_formals.count(cur_formal_name)) {
            class_table->semant_error(filename, cur_formal) << "Formal parameter " << cur_formal_name << " is already defined.\n";
            continue;
        }
        seen_formals.insert(cur_formal_name);

        TypeInfo *info = new TypeInfo();
        info->type = cur_formal_type;
        info->object = NULL;
        env->addid(cur_formal_name, info);
    }

    // Continue recursion
    expr->typecheck(class_table, env, current_class);

    // Assert that the body's type is the return type (or a subclass)
    Symbol body_type = expr->get_type();
    if (!class_table->is_subclass_given_context(body_type, return_type, current_class)) {
        class_table->semant_error(filename, this) << "Actual return type " << body_type << " of method " << name << " does not match declared return type " << return_type << ".\n";
    }

    env->exitscope();
}
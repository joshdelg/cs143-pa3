

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
    std::vector<bool> class_registered;

    // Only use the global scope for the ClassTable
    enterscope();

    // Must call after enterscope because cannot add classes to the table without a scope
    install_basic_classes();

    // Build flat SymbolTable of our classes, essentially a mapping from class name to orphan InheritanceNodes.
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        Class_ c = classes->nth(i);
        Symbol class_name = c->get_name();

        // Check for redefinition of basic classes (and SELF_TYPE — not a legal class name)
        if (class_name == Int || class_name == Bool || class_name == Str || class_name == SELF_TYPE) {
            semant_error(c) << "Redefinition of basic class " << class_name << ".\n";
            class_registered.push_back(false);
            continue;
        }

        // Check for redefinition of user-defined classes
        if (probe(class_name) != NULL) {
            semant_error(c) << "Class " << class_name << " was previously defined.\n";
            class_registered.push_back(false);
            continue;
        }

        // Otherwise, add the class to the SymbolTable
        InheritanceNodeP new_node = new InheritanceNode();
        new_node->class_node = c;
        new_node->name = class_name;
        new_node->parent = NULL;
        addid(class_name, new_node);
        class_registered.push_back(true);
    }

    /* Link our InheritanceNodes together. */
    int reg_idx = 0;
    for (int i = classes->first(); classes->more(i); i = classes->next(i), reg_idx++) {
        if (!class_registered[reg_idx]) {
            continue;
        }
        Class_ c = classes->nth(i);
        Symbol parent_name = c->get_parent();

        InheritanceNodeP child_node = lookup(c->get_name());
        InheritanceNodeP parent_node = lookup(parent_name);
        // Must come before NULL check becuase SELF_TYPE doesn't actually exist
        if (parent_name == SELF_TYPE) {
            semant_error(c) << "Class " << c->get_name() << " cannot inherit class SELF_TYPE.\n";
            continue;
        }

        if (parent_node == NULL) {
            semant_error(c) << "Class " << c->get_name() << " inherits from an undefined class " << parent_name
                            << ".\n";
            continue;
        }

        if (parent_name == Int || parent_name == Bool || parent_name == Str) {
            semant_error(c) << "Class " << c->get_name() << " cannot inherit class " << parent_name << ".\n";
            continue;
        }

        child_node->parent = parent_node;
    }

    /* Check for cycles: walk parent pointers; more steps than there are
       classes (including built-ins) means we are stuck in a cycle. */
    size_t total_classes = gettable().front().size();
    reg_idx = 0;
    for (int i = classes->first(); classes->more(i); i = classes->next(i), reg_idx++) {
        if (!class_registered[reg_idx]) {
            continue;
        }
        Class_ c = classes->nth(i);
        InheritanceNodeP current_node = lookup(c->get_name());

        size_t moves = 0;
        while (current_node != NULL) {
            current_node = current_node->parent;
            moves++;
            if (moves > total_classes) {
                semant_error(c) << "Class " << c->get_name() << ", or an ancestor of " << c->get_name()
                                << ", is involved in an inheritance cycle.\n";
                break;
            }
        }
    }

    // print_debug_hierarchy();

    /* ClassTable should be complete */
    return;
}

void ClassTable::check_main_class_and_method() {
    InheritanceNode *main_node = lookup(Main);
    if (main_node == NULL) {
        semant_error() << "Class Main is not defined.\n";
    } else {
        Feature main_method = lookup_method(main_meth, Main, Main);
        if (main_method == NULL) {
            semant_error(main_node->class_node) << "No 'main' method in class Main.\n";
        } else if (main_method->get_formals()->len() != 0) {
            semant_error(main_node->class_node) << "'main' method in class Main should have no arguments.\n";
        }
    }
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

void ClassTable::collect_methods_and_attributes(Classes classes) {
    ScopeList& scope_list = gettable();
    if (scope_list.empty()) {
        cout << "No scopes in ClassTable" << endl;
        return;
    }

    std::unordered_set<Symbol> initialized;

    // Iterate in program class order, not symbol table order to match error messages
    for (int ci = classes->first(); classes->more(ci); ci = classes->next(ci)) {
        Class_ prog_class = classes->nth(ci);
        InheritanceNodeP node = lookup(prog_class->get_name());
        if (node == NULL) {
            continue;
        }
        if (initialized.count(node->name)) {
            continue;
        }

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

Feature ClassTable::lookup_method(Symbol name, Symbol class_name, Symbol current_class) {
    // NULL if class not found
    InheritanceNode *node = lookup_in_context(class_name, current_class);
    if (node == NULL) return NULL;

    // ... or if method not found (.find allows a safe lookup)
    auto iter = node->methods.find(name);
    if (iter == node->methods.end()) return NULL;

    // iter entry -> FeatureOverrideInfo -> feature
    return iter->second->feature;

}


Feature ClassTable::lookup_attribute(Symbol name, Symbol class_name, Symbol current_class) {
    // NULL if class not found
    InheritanceNode *node = lookup_in_context(class_name, current_class);
    if (node == NULL) return NULL;

    // ... or if attribute not found (.find allows a safe lookup)
    auto it = node->attributes.find(name);
    if (it == node->attributes.end()) return NULL;

    // it entry -> FeatureOverrideInfo -> feature
    return it->second->feature;
}

InheritanceNode *ClassTable::lookup_in_context(Symbol type_name, Symbol current_class) {
    return lookup(normalize_maybe_self_type(type_name, current_class));
}

Symbol ClassTable::normalize_maybe_self_type(Symbol t, Symbol current_class) {
    if (t == SELF_TYPE) {
        return current_class;
    }
    return t;
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
    Symbol actual_a = normalize_maybe_self_type(a, C);
    Symbol actual_b = normalize_maybe_self_type(b, C);

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

Symbol ClassTable::class_join_in_context(Symbol a, Symbol b, Symbol C) {
    Symbol actual_a = normalize_maybe_self_type(a, C);
    Symbol actual_b = normalize_maybe_self_type(b, C);

    return class_join(actual_a, actual_b);
}


/*** AST Method Implementations */

bool method_class::register_method_or_attribute(std::unordered_map<Symbol, FeatureOverrideInfoP> &methods,
                                                std::unordered_map<Symbol, FeatureOverrideInfoP> &attributes,
                                                std::string &error_message, Class_ current_class) {
    (void)attributes;
    const char *n = name->get_string();
    FeatureOverrideInfoP existing = methods[name];

    if (existing != NULL) {
        if (existing->owner_class == current_class) {
            error_message = std::string("Method ") + n + " is multiply defined.";
            return false;
        }
        
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

   Environment *env = new Environment();
   if (classtable->errors() == 0) {
     classtable->collect_methods_and_attributes(classes);
     classtable->check_main_class_and_method();
     typecheck(classtable, env);
   }

    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
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
    Symbol filename = class_table->lookup(current_class)->class_node->get_filename(); // for error messages
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
            class_table->semant_error(filename, cur_formal) << "Formal parameter " << cur_formal_name << " is multiply defined.\n";
            continue;
        }
        seen_formals.insert(cur_formal_name);

        TypeInfo *info = new TypeInfo();
        info->type = cur_formal_type;
        info->object = NULL;
        env->addid(cur_formal_name, info);
    }

    // Check that the return type exists
    // If not, set return type to Object to avoid redundant error with body typecheck
    InheritanceNode *return_node = class_table->lookup_in_context(return_type, current_class);
    if (return_node == NULL) {
        class_table->semant_error(filename, this) << "Undefined return type " << return_type << " in method " << name << ".\n";
        this->return_type = Object;
    }

    // Typecheck body
    expr->typecheck(class_table, env, current_class);

    // Assert that the body's type is the return type (or a subclass)
    Symbol body_type = expr->get_type();

    if (body_type != No_type) {
        if (!class_table->is_subclass_given_context(body_type, return_type, current_class)) {
            class_table->semant_error(filename, this) << "Inferred return type " << body_type << " of method " << name << " does not conform to declared return type " << return_type << ".\n";
        }
    }

    env->exitscope();
}


void attr_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    Symbol filename = class_table->lookup(current_class)->class_node->get_filename();

    //  [Attr-Init]
    //
    //   O_C(x) = T_0
    //   O_C[SELF_TYPE_C / self], M, C |- e_1 : T_1
    //   T_1 <= T_0
    //  -------------------------------------------------------
    //   O_C, M, C |- x : T_0 <- e_1;

    // Get the type T_1 for e_1
    init->typecheck(class_table, env, current_class);

    // Ensure T_1 <= T_0
    if (init->get_type() != No_type) {
        if (!class_table->is_subclass_given_context(init->get_type(), type_decl, current_class)) {
            class_table->semant_error(filename, this)
                << "Inferred type " << init->get_type() << " of initialization of attribute " << name
                << " does not conform to declared type "
                << class_table->normalize_maybe_self_type(type_decl, current_class) << ".\n";
        }
    }
   
    //  [Attr-No-Init]
    //
    //   O_C(x) = T
    //  -------------------------------------------------------
    //   O_C, M, C |- x : T

    // init is no_expr, which turns into No_type, which code above handles regularly.
}

void no_expr_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    type = No_type;
}

void int_const_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Int]
    //
    //   i is an integer constant
    //  ----------------------------------
    //   O, M, C |- i : Int

    type = Int;
}

void bool_const_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [True]
    // 
    //  ----------------------------------
    //   O, M, C |- true : Bool

    //  [False]
    //
    //  ----------------------------------
    //   O, M, C |- false : Bool

    type = Bool;
}

void string_const_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [String]
    //
    //   s is a string constant
    //  ----------------------------------
    //   O, M, C |- s : String

    type = Str;
}

void isvoid_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Isvoid]
    //
    //   O, M, C |- e_1 : T_1
    //  -------------------------------------------------------
    //   O, M, C |- isvoid e_1 : Bool

    e1->typecheck(class_table, env, current_class);
    type = Bool;
}

void plus_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [Arith]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {*, +, -, /}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Int

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " + " << e2->get_type() << "\n";
    }

    type = Int;
}

void sub_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [Arith]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {*, +, -, /}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Int

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " - " << e2->get_type() << "\n";
    }

    type = Int;
}

void mul_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [Arith]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {*, +, -, /}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Int

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " * " << e2->get_type() << "\n";
    }

    type = Int;
}

void divide_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [Arith]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {*, +, -, /}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Int

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " / " << e2->get_type() << "\n";
    }

    type = Int;
}

void branch_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Case]
    //
    //   O, M, C |- e_0 : T_0
    //   O[T_1 / x_1], M, C |- e_1 : T_1'
    //        ...
    //   O[T_n / x_n], M, C |- e_n : T_n'
    //  -------------------------------------------------------
    //   O, M, C |- case e_0 of
    //                 x_1 : T_1 => e_1;
    //                 ...
    //                 x_n : T_n => e_n;
    //              esac : |_|_{1 <= i <= n} T_i'

    // The body of a case statement is type checked in an environment extended 
    // with the branch's variable bound to the branch's declared type.
    env->enterscope();

    // Practically a new attribute from the body's perspective
    TypeInfo *expr_info = new TypeInfo();
    expr_info->type = type_decl;
    expr_info->object = NULL;
    env->addid(name, expr_info);

    // body
    expr->typecheck(class_table, env, current_class);

    env->exitscope();
}

void new__class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [New]
    //
    //   T' = {  SELF_TYPE_C   if T = SELF_TYPE
    //           T             otherwise
    //  ----------------------------------
    //   O, M, C |- new T : T'

    InheritanceNode *node = class_table->lookup_in_context(this->type_name, current_class);

    if (node == NULL) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "'new' used with undefined class " << this->type_name << ".\n";

        this->set_type(No_type);
        return;
    }

    // Still need to special case SELF_TYPE because static type of new SELF_TYPE is still SELF_TYPE
    if (this->type_name == SELF_TYPE) {
        this->set_type(SELF_TYPE);
    } else {
        this->set_type(node->class_node->get_name());
    }
}

void object_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    TypeInfo *info = env->lookup(name);
    
    if (info == NULL) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Undeclared identifier " << name << ".\n";

        this->set_type(No_type);
        return;
    }

    this->set_type(info->type);
}

void neg_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) { 
    //  [Neg]
    //
    //   O, M, C |- e_1 : Int
    //  -------------------------------------------------------
    //   O, M, C |- ~e_1 : Int

    e1->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Argument of Negation has type " << e1->get_type() << " instead of Int.\n";
    }

    type = Int;
}

void lt_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    // "less-than"

    //  [Compare]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {<, <=}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Bool


    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " < " << e2->get_type() << "\n";
    }

    type = Bool;
}

void leq_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    // "less-than or equal-to"
    
    //  [Compare]
    //
    //   O, M, C |- e_1 : Int
    //   O, M, C |- e_2 : Int
    //   op in {<, <=}
    //  -------------------------------------------------------
    //   O, M, C |- e_1 op e_2 : Bool

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    if (e1->get_type() != Int || e2->get_type() != Int) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "non-Int arguments: " << e1->get_type() << " <= " << e2->get_type() << "\n";
    }

    type = Bool;
}

void comp_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    // "complement"

    //  [Not]
    //
    //   O, M, C |- e_1 : Bool
    //  -------------------------------------------------------
    //   O, M, C |- not e_1 : Bool

    e1->typecheck(class_table, env, current_class);

    if (e1->get_type() != Bool) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Argument of Not has type " << e1->get_type() << " instead of Bool.\n";
    }
    
    type = Bool;
}

void block_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //   [Sequence]
    //
    //   O, M, C |- e_1 : T_1
    //   O, M, C |- e_2 : T_2
    //        ...
    //   O, M, C |- e_n : T_n
    //  -------------------------------------------------------
    //   O, M, C |- { e_1; e_2; ... e_n; } : T_n

    // type of last expression (iterate through each expression)
    Symbol block_type = Object;
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        Expression e_i = body->nth(i);
        e_i->typecheck(class_table, env, current_class);
        block_type = e_i->get_type();
    }
    type = block_type;
}

void loop_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Loop]
    //
    //   O, M, C |- e_1 : Bool
    //   O, M, C |- e_2 : T_2
    //  -------------------------------------------------------
    //   O, M, C |- while e_1 loop e_2 pool : Object

    pred->typecheck(class_table, env, current_class);

    if (pred->get_type() != Bool) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Loop predicate does not have type Bool.\n";
    }

    body->typecheck(class_table, env, current_class);

    // While loops always evaluate to void, which we label as type Object.
    type = Object;
}

void eq_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Equal]
    //
    //   O, M, C |- e_1 : T_1
    //   O, M, C |- e_2 : T_2
    //   T_1 in {Int, String, Bool} OR T_2 in {Int, String, Bool}
    //       ==>  T_1 = T_2
    //  -------------------------------------------------------
    //   O, M, C |- e_1 = e_2 : Bool

    e1->typecheck(class_table, env, current_class);
    e2->typecheck(class_table, env, current_class);

    Symbol T_1 = e1->get_type();
    Symbol T_2 = e2->get_type();

    if ((T_1 == Int || T_1 == Str || T_1 == Bool) || (T_2 == Int || T_2 == Str || T_2 == Bool)) {
        if (T_1 != T_2) {
            class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
                << "Illegal comparison with a basic type.\n";
        }
    }

    type = Bool;
}

void dispatch_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Dispatch]
    //
    //   O, M, C |- e_0 : T_0
    //   O, M, C |- e_1 : T_1
    //        ...
    //   O, M, C |- e_n : T_n
    //   T_0' = {  C     if T_0 = SELF_TYPE_C
    //             T_0   otherwise
    //   M(T_0', f) = (T_1', ..., T_n', T_{n+1}')
    //   T_i <= T_i'    for 1 <= i <= n
    //   T_{n+1} = {  T_0           if T_{n+1}' = SELF_TYPE
    //                T_{n+1}'      otherwise
    //  -------------------------------------------------------
    //   O, M, C |- e_0.f(e_1, ..., e_n) : T_{n+1}


    // First, everything needs a type.
    // Typecheck the reciever (e0)
    expr->typecheck(class_table, env, current_class);

    // Typecheck the args
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        actual->nth(i)->typecheck(class_table, env, current_class);
    }

    // Lookup method on e0's class -- will resolve SELF_TYPE to current class automatically
    Feature method = class_table->lookup_method(name, expr->get_type(), current_class);

    // Catches both method doesn't exist on class and the receiver isn't a valid class
    if (method == NULL) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Dispatch to undefined method " << name << ".\n";
        this->set_type(No_type);
        return;
    }

    // Check number of arguments
    if (actual->len() != method->get_formals()->len()) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Method " << name << " called with wrong number of arguments.\n";
        this->set_type(No_type);
        return;
    }

    // Typecheck each actual argument
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
        Symbol actual_arg_type = actual->nth(i)->get_type();
        Symbol formal_arg_type = method->get_formals()->nth(i)->get_type();
        Symbol formal_arg_name = method->get_formals()->nth(i)->get_name();

        if (!class_table->is_subclass_given_context(actual_arg_type, formal_arg_type, current_class)) {
            class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
                << "In call of method " << name << ", type " << actual_arg_type << " of parameter " << formal_arg_name << " does not conform to declared type "
                << formal_arg_type << ".\n";

            /* Continue typechecking args */
        }
    }

    // Set the dispatch's type to the function's normalized return type:
    // i.e. if f returns SELF_TYPE, the type of the dispatch is the *unnormalized* type of the receiver
    this->set_type(class_table->normalize_maybe_self_type(method->get_type(), expr->get_type()));
}

void static_dispatch_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [StaticDispatch]
    //
    //   O, M, C |- e_0 : T_0
    //   O, M, C |- e_1 : T_1
    //        ...
    //   O, M, C |- e_n : T_n
    //   T_0 <= T
    //   M(T, f) = (T_1', ..., T_n', T_{n+1}')
    //   T_i <= T_i'    for 1 <= i <= n
    //   T_{n+1} = {  T_0           if T_{n+1}' = SELF_TYPE
    //                T_{n+1}'      otherwise
    //  -------------------------------------------------------
    //   O, M, C |- e_0@T.f(e_1, ..., e_n) : T_{n+1}

    return;
}

void assign_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [ASSIGN]
    //
    //   O(Id) = T
    //   O, M, C |- e_1 : T'
    //   T' <= T
    //  ----------------------------------
    //   O, M, C |- Id <- e_1 : T'

    TypeInfo *info = env->lookup(name);
    if (info == NULL) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Assignment to undeclared identifier " << name << ".\n";
        expr->typecheck(class_table, env, current_class);
        type = Object;
        return;
    }

    expr->typecheck(class_table, env, current_class);
    Symbol expr_type = expr->get_type();

    if (!class_table->is_subclass_given_context(expr_type, info->type, current_class)) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Type " << expr_type << " of assigned expression does not conform to declared type " << info->type << " of identifier " << name << ".\n";
    }

    type = expr_type;
}

void cond_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [If]
    //
    //   O, M, C |- e_1 : Bool
    //   O, M, C |- e_2 : T_2
    //   O, M, C |- e_3 : T_3
    //  -------------------------------------------------------
    //   O, M, C |- if e_1 then e_2 else e_3 fi : T_2 |_| T_3

    pred->typecheck(class_table, env, current_class);

    if (pred->get_type() != Bool) {
        class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
            << "Predicate of conditional does not have type Bool.\n";
    }

    then_exp->typecheck(class_table, env, current_class);
    else_exp->typecheck(class_table, env, current_class);

    type = class_table->class_join_in_context(then_exp->get_type(), else_exp->get_type(), current_class);
}

void let_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    // [Let-Init]
    //
    //   T_0' = {  SELF_TYPE_C   if T_0 = SELF_TYPE
    //             T_0           otherwise
    //   O, M, C |- e_1 : T_1
    //   T_1 <= T_0'
    //   O[T_0' / x], M, C |- e_2 : T_2
    //  -------------------------------------------------------
    //   O, M, C |- let x : T_0 <- e_1 in e_2 : T_2

    // T_0'
    init->typecheck(class_table, env, current_class);
    
    // T_1 <= T_0'
    if (init->get_type() != No_type) {
        if (!class_table->is_subclass_given_context(init->get_type(), type_decl, current_class)) {
            class_table->semant_error(class_table->lookup(current_class)->class_node->get_filename(), this)
                << "Inferred type " << init->get_type() << " of initialization of " << identifier << " does not conform to declared type "
                << class_table->normalize_maybe_self_type(type_decl, current_class) << ".\n";
        }
    }

    // Now enter new scope and check body
    env->enterscope();

    TypeInfo *info = new TypeInfo();
    info->type = type_decl;
    info->object = NULL;
    env->addid(identifier, info);

    body->typecheck(class_table, env, current_class);
    type = body->get_type();

    env->exitscope();

    //   [Let-No-Init]
    //
    //   T_0' = {  SELF_TYPE_C   if T_0 = SELF_TYPE
    //             T_0           otherwise
    //   O[T_0' / x], M, C |- e_1 : T_1
    //  -------------------------------------------------------
    //   O, M, C |- let x : T_0 in e_1 : T_1

    // similar pattern to attr_class, the init->get_type != No_type check handles both cases.
}

void typcase_class::typecheck(ClassTable *class_table, Environment *env, Symbol current_class) {
    //  [Case]
    //
    //   O, M, C |- e_0 : T_0
    //   O[T_1 / x_1], M, C |- e_1 : T_1'
    //        ...
    //   O[T_n / x_n], M, C |- e_n : T_n'
    //  -------------------------------------------------------
    //   O, M, C |- case e_0 of
    //                 x_1 : T_1 => e_1;
    //                 ...
    //                 x_n : T_n => e_n;
    //              esac : |_|_{1 <= i <= n} T_i'

    expr->typecheck(class_table, env, current_class);
    Symbol result_type = No_type;

    for (int i = cases->first(); cases->more(i); i = cases->next(i)) {
        Case c_i = cases->nth(i);
        c_i->typecheck(class_table, env, current_class);

        if (result_type == No_type) {
            result_type = c_i->get_type();
        } else {
            result_type = class_table->class_join_in_context(result_type, c_i->get_type(), current_class);
        }
    }

    type = result_type;
}
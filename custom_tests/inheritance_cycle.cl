-- inheritance cycle: A -> B -> C -> A, plus a self-loop on D

class A inherits B { };
class B inherits C { };
class C inherits A { };

class D inherits D { };

class Main {
    main() : Object { 0 };
};

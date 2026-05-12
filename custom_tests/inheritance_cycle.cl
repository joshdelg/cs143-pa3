-- inheritance cycle: A -> B -> C -> A, plus a self-loop on D

class A inherits B {
    foo(): Int {1};
};

class B inherits C {
    bar(): Int {2};
};
class C inherits A {
    baz(): Bool {true};
};

class D inherits D {
    hello(): Bool {true};
    hello: String <- "Hello!";
};

class Main {
    main() : Object { 0 };
};

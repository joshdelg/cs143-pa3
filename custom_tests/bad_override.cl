class Main {
    main() : Bool {
        true
    };
};

class A {
    foo() : Bool {
        (true)
    };

    bar: Int <- 1;
};

class B inherits A {
    foo(extraParam: Int) : Bool {
        (false)
    };

    bar: Int <- 2;
};
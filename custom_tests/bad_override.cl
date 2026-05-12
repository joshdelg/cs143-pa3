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
    -- Redef different args
    foo(extraParam: Int) : Bool {
        (false)
    };

    bar: Int <- 2;
};

class C inherits A {
    -- Redef with same args diff return
    foo(): Int {
        5
    };

    bar: Bool <- false;
};
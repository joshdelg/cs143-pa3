class Main {
    -- Test using attribute from class scope
    main(): Int {
        myVar
    };

    myVar: Int <- 100;

    -- test name resolution from formal param
    myMethod(arg1: Bool): Bool {
        arg1
    };

    -- multiple formals
    myMethod2(arg1: Bool, arg2: String) : String {
        arg2
    };

    -- formal hides attr
    x: Int <- 5;
    formalHidesAttr(x: String): String {
        x
    };

    -- method and attr with same name
    foo: Int <- 7;
    foo(): Int { foo + 1 };

    -- attribute init with dispatch
    counterStart: Int <- compute() + myVar;
    compute(): Int { 1 + 1 };

    -- mutually recursive
    even(n: Int): Bool {
        if n = 0 then true else odd(n - 1) fi
    };
    odd(n: Int): Bool {
        if n = 0 then false else even(n - 1) fi
    };
};
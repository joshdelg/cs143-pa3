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
};
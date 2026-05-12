class Main {
    main(): Int {
        undefinedVar
    };

    foo(a: Int): Bool {
        true
    };

    -- use var defined in other method's formal
    bar(b: Int): Int {
        a
    };
};
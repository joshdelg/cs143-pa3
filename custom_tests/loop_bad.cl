class Main {
    main(): Object { self };

    -- Predicate is an Int literal (not Bool)
    bad1(): Object {
        while 1 loop 0 pool
    };

    -- Predicate is a String literal (not Bool)
    bad2(): Object {
        while "hello" loop 0 pool
    };

    -- Predicate is a fresh object (Main is not Bool)
    bad3(): Object {
        while new Main loop 0 pool
    };

    -- Predicate is an arithmetic result (Int, not Bool)
    bad4(): Object {
        while 1 + 1 loop 0 pool
    };

    -- Predicate is the integer complement of an Int (~Int = Int, not Bool)
    bad5(): Object {
        while ~0 loop 0 pool
    };

    -- Predicate is correct (Bool) but body contains a type error
    bad6(): Object {
        while false loop 1 + "bad" pool
    };

    -- Predicate is correct but body has an undeclared identifier
    bad7(): Object {
        while false loop undeclaredId pool
    };
};

class Main {
    main(): Object { self };

    -- Predicates must be bool
    bad1(): Object {
        while 1 loop 0 pool
    };

    bad2(): Object {
        while "hello" loop 0 pool
    };

    bad3(): Object {
        while new Main loop 0 pool
    };

    bad4(): Object {
        while 1 + 1 loop 0 pool
    };

    bad5(): Object {
        while ~0 loop 0 pool
    };

    -- body contains type error
    bad6(): Object {
        while false loop 1 + "bad" pool
    };

    bad7(): Object {
        while false loop undeclaredId pool
    };

    -- predicate is No_type (cascade)
    bad8(): Object {
        while undeclaredId loop 0 pool
    };
};

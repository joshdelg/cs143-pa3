class Main {
    main(): Object { self };

    test1(): Object {
        while false loop 0 pool
    };

    -- body may have any static type; result is still Object
    test2(): Object {
        while false loop "any type is fine" pool
    };

    -- assignment in body
    test3(): Object {
        let i: Int <- 0 in
            while i < 5 loop
                i <- i + 1
            pool
    };

    -- assign loop result
    test4(): Object {
        let result: Object <- (while false loop 0 pool) in
            result
    };

    -- nested
    test5(): Object {
        while false loop
            while false loop 0 pool
        pool
    };

    -- block body
    test6(): Object {
        let x: Int <- 10 in
            while 0 < x loop {
                x <- x - 1;
                "iteration";
            }
            pool
    };

    test7(): Object {
        while not true loop 0 pool
    };

    test8(): Object {
        let obj: Main <- new Main in
            while isvoid obj loop 0 pool
    };

    -- test shadowing
    test9(): Int {
        let x: Int <- 5 in {
            while false loop
                let x: String <- "inner" in x
            pool;
            x + 1;
        }
    };

    -- predicate is a method call that returns Bool
    isReady(): Bool { false };
    test10(): Object {
        while isReady() loop 0 pool
    };
};

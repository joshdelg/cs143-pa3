class Main {
    main(): Object { self };

    -- Simplest loop: false predicate, result is Object
    test1(): Object {
        while false loop 0 pool
    };

    -- Body may have any static type; result is still Object
    test2(): Object {
        while false loop "any type is fine" pool
    };

    -- Counting loop; body assigns to a let variable
    test3(): Object {
        let i: Int <- 0 in
            while i < 5 loop
                i <- i + 1
            pool
    };

    -- Loop result assigned to an Object variable (Object <= Object)
    test4(): Object {
        let result: Object <- (while false loop 0 pool) in
            result
    };

    -- Nested loops; outer result is Object
    test5(): Object {
        while false loop
            while false loop 0 pool
        pool
    };

    -- Predicate from a comparison; body uses a block (all exprs need semicolons)
    test6(): Object {
        let x: Int <- 10 in
            while 0 < x loop {
                x <- x - 1;
                "iteration";
            }
            pool
    };

    -- Predicate from not (Bool)
    test7(): Object {
        while not true loop 0 pool
    };

    -- Predicate from isvoid (Bool)
    test8(): Object {
        let obj: Main <- new Main in
            while isvoid obj loop 0 pool
    };
};

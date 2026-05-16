class Main {
    x: Int <- 0;

    main(): Object { self };

    -- Type of a block is the type of its last expression
    test1(): Int {
        { true; "hello"; 42; }
    };

    -- Single-expression block; type equals that expression's type
    test2(): Bool {
        { true; }
    };

    -- Side-effecting assignments; last expression gives the block type
    test3(): Int {
        { x <- 5; x + 1; }
    };

    -- Nested block; outer type comes from the inner block's last expr
    test4(): String {
        { 1; { true; "result"; }; }
    };

    -- Block result used in arithmetic
    test5(): Int {
        { 1; 2; 3; } + 4
    };

    -- Attribute initializer is a block; result is the last expr (Int)
    computed: Int <- { 1; 2; 3; };

    -- Block containing a loop (Object) followed by an Int
    test6(): Int {
        { while false loop 0 pool; 99; }
    };

    -- Block with five expressions; only the last type is the block's type
    test7(): Bool {
        { 1; "two"; 3; new Main; false; }
    };

    -- Block result bound in a let
    test8(): Int {
        let v: Int <- { 10; 20; 30; } in v
    };
};

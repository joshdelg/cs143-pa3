class Main {
    x: Int <- 0;

    main(): Object { self };

    test1(): Int {
        { true; "hello"; 42; }
    };

    test2(): Bool {
        { true; }
    };

    test3(): Int {
        { x <- 5; x + 1; }
    };

    -- nested block
    test4(): String {
        { 1; { true; "result"; }; }
    };

    -- block returns last statement
    test5(): Int {
        { 1; 2; 3; } + 4
    };

    -- assign block return value
    computed: Int <- { 1; 2; 3; };

    -- include loop
    test6(): Int {
        { while false loop 0 pool; 99; }
    };

    test7(): Bool {
        { 1; "two"; 3; new Main; false; }
    };

    test8(): Int {
        let v: Int <- { 10; 20; 30; } in v
    };

    -- self = SELF_TYPE
    test9(): SELF_TYPE {
        { 1; self; }
    };

    test10(): Object {
        { 1; "two"; self; }
    };

    test11(): SELF_TYPE {
        { 1; new SELF_TYPE; }
    };

    test12(): Int {
        { let s: SELF_TYPE <- self in s; 99; }
    };

    selfAttr: SELF_TYPE <- { 1; self; };
};

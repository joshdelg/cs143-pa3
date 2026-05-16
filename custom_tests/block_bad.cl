class Main {
    main(): Object { self };

    -- type error in block
    bad1(): Int {
        { 1 + "hello"; 0; }
    };

    -- wrong return type
    bad2(): String {
        { 1; 2; 3; }
    };

    -- undecl
    bad3(): Object {
        { undeclared; 0; }
    };

    -- type error comparioson
    bad4(): Object {
        { 1 <= true; 0; }
    };

    -- type error not
    bad5(): Bool {
        { not 42; }
    };

    -- type error block type
    bad6(): Bool {
        { true; "middle"; 99; }
    };

    -- assignto undecl
    bad7(): Object {
        { noSuchVar <- 5; 0; }
    };

    -- can't assign to self in block
    bad8(): Object {
        { self <- new Main; 0; }
    };

    -- SELF_TYPE block type isn't int
    bad9(): Int {
        { 1; self; }
    };

    -- Main != SELF_TYPE
    bad10(): SELF_TYPE {
        { 1; new Main; }
    };

    -- can't bind self
    bad11(): Object {
        { let self: Int <- 1 in self; 0; }
    };
};

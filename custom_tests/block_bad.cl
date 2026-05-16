class Main {
    main(): Object { self };

    -- Arithmetic type error inside a block (String is not Int)
    bad1(): Int {
        { 1 + "hello"; 0; }
    };

    -- Block's last expression type (Int) does not conform to declared return type (String)
    bad2(): String {
        { 1; 2; 3; }
    };

    -- Undeclared identifier referenced inside a block
    bad3(): Object {
        { undeclared; 0; }
    };

    -- Comparison type error inside a block (Bool is not Int for <=)
    bad4(): Object {
        { 1 <= true; 0; }
    };

    -- not applied to Int (not Bool) inside a block
    bad5(): Bool {
        { not 42; }
    };

    -- Block last expression is Int but method declares Bool
    bad6(): Bool {
        { true; "middle"; 99; }
    };

    -- Block containing an assignment to an undeclared variable
    bad7(): Object {
        { noSuchVar <- 5; 0; }
    };
};

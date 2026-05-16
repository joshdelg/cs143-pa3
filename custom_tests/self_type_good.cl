class Counter {
    count: Int <- 0;

    -- Return type SELF_TYPE
    increment(): SELF_TYPE {
        { count <- count + 1; self; }
    };

    getCount(): Int { count };

    -- new SELF_TYPE valid
    fresh(): SELF_TYPE {
        new SELF_TYPE
    };
};

class SpecialCounter inherits Counter {};

class Main {
    -- SELF_TYPE is type of SELF
    selfRef: SELF_TYPE <- self;

    main(): Object {
        -- incremeent returns SpecialCounter conforms to Counter
        let c: Counter <- (new SpecialCounter).increment() in
            c
    };

    test1(): SELF_TYPE {
        let s: SELF_TYPE <- self in s
    };

    test2(): SELF_TYPE {
        new SELF_TYPE
    };

    test3(): SELF_TYPE {
        self
    };

    test4(): SELF_TYPE {
        selfRef <- new SELF_TYPE
    };

    test5(): SELF_TYPE {
        let s: SELF_TYPE <- new SELF_TYPE in s
    };

    test6(): Object {
        new SELF_TYPE
    };
};

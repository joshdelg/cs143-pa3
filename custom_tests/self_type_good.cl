class Counter {
    count: Int <- 0;

    -- Return type SELF_TYPE; body returns self (SELF_TYPE_Counter <= SELF_TYPE_Counter)
    increment(): SELF_TYPE {
        { count <- count + 1; self; }
    };

    getCount(): Int { count };

    -- new SELF_TYPE is valid; static type of new SELF_TYPE is SELF_TYPE
    fresh(): SELF_TYPE {
        new SELF_TYPE
    };
};

class SpecialCounter inherits Counter {
    -- Inherits increment(); in this class SELF_TYPE means SpecialCounter
};

class Main {
    -- Attribute declared with type SELF_TYPE; initialized with self
    selfRef: SELF_TYPE <- self;

    main(): Object {
        -- increment() on SpecialCounter returns SELF_TYPE_SpecialCounter = SpecialCounter
        -- SpecialCounter conforms to Counter
        let c: Counter <- (new SpecialCounter).increment() in
            c
    };

    -- let variable declared with SELF_TYPE; self conforms to SELF_TYPE_Main
    test1(): SELF_TYPE {
        let s: SELF_TYPE <- self in s
    };

    -- new SELF_TYPE -> static type is SELF_TYPE (not Main)
    test2(): SELF_TYPE {
        new SELF_TYPE
    };

    -- Method body is self (SELF_TYPE <= SELF_TYPE)
    test3(): SELF_TYPE {
        self
    };

    -- Assigning new SELF_TYPE to a SELF_TYPE attribute; result type is SELF_TYPE
    test4(): SELF_TYPE {
        selfRef <- new SELF_TYPE
    };

    -- Let with SELF_TYPE type and new SELF_TYPE init; body yields SELF_TYPE
    test5(): SELF_TYPE {
        let s: SELF_TYPE <- new SELF_TYPE in s
    };

    -- SELF_TYPE_Main (= Main) conforms to Object
    test6(): Object {
        new SELF_TYPE
    };
};

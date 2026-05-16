class A {};

class Main {
    obj: A <- new A;

    main(): Object { self };

    test1(): Bool {
        isvoid 5
    };

    test2(): Bool {
        isvoid "hello"
    };

    test3(): Bool {
        isvoid new A
    };

    test4(): Bool {
        isvoid self
    };

    -- isvoid returns bool
    test5(): Int {
        if isvoid new A then 0 else 1 fi
    };

    -- not (isvoid ...) -> Bool
    test6(): Bool {
        not isvoid 5
    };

    -- isvoid on an attribute reference -> Bool
    test7(): Bool {
        isvoid obj
    };

    -- isvoid bool as while predicate
    test8(): Object {
        while isvoid obj loop 0 pool
    };

    test9(): Bool {
        let x: A <- new A in isvoid x
    };

    -- nested isvoid
    test10(): Bool {
        isvoid (isvoid 0)
    };
};

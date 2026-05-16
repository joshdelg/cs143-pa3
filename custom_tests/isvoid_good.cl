class A {};

class Main {
    obj: A <- new A;

    main(): Object { self };

    -- isvoid on an Int constant; always false at runtime but statically valid -> Bool
    test1(): Bool {
        isvoid 5
    };

    -- isvoid on a String constant -> Bool
    test2(): Bool {
        isvoid "hello"
    };

    -- isvoid on a fresh object (always false) -> Bool
    test3(): Bool {
        isvoid new A
    };

    -- isvoid on self -> Bool
    test4(): Bool {
        isvoid self
    };

    -- isvoid result drives an if predicate (Bool is correct for if)
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

    -- isvoid result as the while predicate -> loop returns Object
    test8(): Object {
        while isvoid obj loop 0 pool
    };

    -- isvoid on a let-bound variable -> Bool
    test9(): Bool {
        let x: A <- new A in isvoid x
    };

    -- Nested isvoid (isvoid Bool -> Bool)
    test10(): Bool {
        isvoid (isvoid 0)
    };
};

class A {};
class B inherits A {};

class Main {
    main(): Object { self };

    -- Both branches same type -> result is that type
    test1(): Int {
        if true then 1 else 2 fi
    };

    -- Branches differ: B and A; join(B, A) = A
    test2(): A {
        if false then new B else new A fi
    };

    -- Nested conditionals; all paths return Int
    test3(): Int {
        if true then
            if false then 1 else 2 fi
        else
            3
        fi
    };

    -- Predicate from a comparison expression (< returns Bool)
    test4(): Int {
        if 1 < 2 then 10 else 20 fi
    };

    -- Branches of unrelated types; join(Int, String) = Object
    test5(): Object {
        if true then 5 else "hello" fi
    };

    -- Predicate from isvoid (always Bool)
    test6(): Int {
        if isvoid new A then 0 else 1 fi
    };

    -- join(SELF_TYPE_Main, Int) = join(Main, Int) = Object
    test7(): Object {
        if true then self else 0 fi
    };

    -- Predicate from not (Bool -> Bool)
    test8(): Int {
        if not false then 1 else 2 fi
    };

    -- Both branches return Bool; join(Bool, Bool) = Bool
    test9(): Bool {
        if true then false else true fi
    };
};

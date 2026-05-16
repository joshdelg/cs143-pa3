class A {};
class B inherits A {};

class Main {
    main(): Object { self };

    -- both branches are int
    test1(): Int {
        if true then 1 else 2 fi
    };

    -- join(B, A) = A
    test2(): A {
        if false then new B else new A fi
    };

    -- nested, return int
    test3(): Int {
        if true then
            if false then 1 else 2 fi
        else
            3
        fi
    };

    -- bool predicate
    test4(): Int {
        if 1 < 2 then 10 else 20 fi
    };

    -- join(Int, String) = Object
    test5(): Object {
        if true then 5 else "hello" fi
    };

    -- bool isvoid predicate
    test6(): Int {
        if isvoid new A then 0 else 1 fi
    };

    -- join(SELF_TYPE_Main, Int) = join(Main, Int) = Object
    test7(): Object {
        if true then self else 0 fi
    };

    -- bool not predicate
    test8(): Int {
        if not false then 1 else 2 fi
    };

    -- join(Bool, Bool) = Bool
    test9(): Bool {
        if true then false else true fi
    };

    -- join(SELF_TYPE_Main, SELF_TYPE_Main) = SELF_TYPE_Main
    test10(): SELF_TYPE {
        if true then self else self fi
    };

    -- join(SELF_TYPE_Main, Main) = join(Main, Main) = Main
    test11(): Main {
        if true then self else new Main fi
    };

    -- Both branches are new SELF_TYPE
    test12(): SELF_TYPE {
        if true then new SELF_TYPE else new SELF_TYPE fi
    };

    -- join(SELF_TYPE_Main, A) = join(Main, A) = Object
    test13(): Object {
        if true then self else new A fi
    };

    -- join correct across inheritance
    test14(): D {
        if true then new D1 else new D2a fi
    };

    test15(): D {
        if false then new D2a else new D1 fi
    };
};

class D {};
class D1 inherits D {};
class D2 inherits D {};
class D2a inherits D2 {};

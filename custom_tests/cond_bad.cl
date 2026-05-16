class Main {
    main(): Object { self };

    helper(): Int { 1 };

    -- predicate must be bool
    bad1(): Int {
        if 5 then 1 else 2 fi
    };

    bad2(): Int {
        if "hello" then 1 else 2 fi
    };

    bad3(): Int {
        if new Main then 1 else 2 fi
    };

    bad4(): Int {
        if 1 + 1 then 1 else 2 fi
    };

    bad5(): Int {
        if helper() then 1 else 2 fi
    };

    bad6(): Int {
        if ~3 then 1 else 2 fi
    };

    -- then branch has type error
    bad7(): Int {
        if true then 1 + "oops" else 2 fi
    };

    -- join(A, B) = A, return type does not conform
    bad8(): B {
        if true then new A else new B fi
    };

    -- else branch has type error
    bad9(): Int {
        if true then 1 else 1 + "oops" fi
    };

    -- predicate is No_type
    bad10(): Int {
        if undeclaredId then 1 else 2 fi
    };
};

class A {};
class B inherits A {};

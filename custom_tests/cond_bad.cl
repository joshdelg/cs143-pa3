class Main {
    main(): Object { self };

    helper(): Int { 1 };

    -- Predicate is Int literal (not Bool)
    bad1(): Int {
        if 5 then 1 else 2 fi
    };

    -- Predicate is String literal (not Bool)
    bad2(): Int {
        if "hello" then 1 else 2 fi
    };

    -- Predicate is a fresh object (Main, not Bool)
    bad3(): Int {
        if new Main then 1 else 2 fi
    };

    -- Predicate is an arithmetic result (Int, not Bool)
    bad4(): Int {
        if 1 + 1 then 1 else 2 fi
    };

    -- Predicate comes from a method that returns Int
    bad5(): Int {
        if helper() then 1 else 2 fi
    };

    -- Predicate is the integer complement of an Int (~ returns Int, not Bool)
    bad6(): Int {
        if ~3 then 1 else 2 fi
    };

    -- Then-branch has a type error independent of the predicate
    bad7(): Int {
        if true then 1 + "oops" else 2 fi
    };
};

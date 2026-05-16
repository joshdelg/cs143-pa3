class Main {
    main(): Object { self };

    -- Attribute declared with an undefined type (no initializer)
    badAttr: UndefinedType;

    -- Attribute with undefined type and an initializer
    badAttrInit: AlsoUndefined <- 0;

    -- Formal parameter with an undefined declared type
    badFormal(x: NoSuchClass): Int {
        0
    };

    -- Multiple formals; only the second has an undefined type
    badFormalMulti(good: Int, bad: MissingClass): Int {
        good
    };

    -- Case branch with an undefined declared type
    badBranch(): Object {
        case 0 of
            x: YetAnotherMissing => 0;
        esac
    };

    -- Undefined return type for a method
    badReturn(): GhostClass {
        0
    };
};

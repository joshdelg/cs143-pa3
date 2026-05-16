class Main {
    main(): Object { self };

    -- Attribute declared with an undefined type
    badAttr: UndefinedType;

    -- undefined type + init
    badAttrInit: AlsoUndefined <- 0;

    -- param with undecl type
    badFormal(x: NoSuchClass): Int {
        0
    };

    -- second has an undefined type
    badFormalMulti(good: Int, bad: MissingClass): Int {
        good
    };

    -- case branch with an undefined type
    badBranch(): Object {
        case 0 of
            x: YetAnotherMissing => 0;
        esac
    };

    -- undefined return type for a method
    badReturn(): GhostClass {
        0
    };
};

class Main {
    main(): Object {
        true
    };

    -- Should use k on A
    inheritedAttrThroughChild(): Int {
        (new B).fromA()
    };

    -- Check both version's use String
    normalThenStaticVersion(): Bool {
        (new B).version() = (new B)@A.version()
    };

    -- should get A's k
    selfStaticToAncestor(): Int {
        (new B).viaStaticSelf()
    };

    -- should typecheck 7 against A's def
    inheritedMethodWithFormal(): Int {
        (new B).addK(7)
    };
};

class A {
    k: Int <- 3;

    fromA(): Int {
        k
    };

    version(): String {
        "A"
    };

    addK(n: Int): Int {
        k + n
    };
};

class B inherits A {
    version(): String {
        "B"
    };

    viaStaticSelf(): Int {
        self@A.fromA()
    };
};

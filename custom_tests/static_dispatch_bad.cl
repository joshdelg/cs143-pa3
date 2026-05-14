class Main {
    main(): Object {
        true
    };

    badReceiverNotSubtype(): Object {
        (new A)@B.bOnly()
    };

    badPrimitiveNotSubtype(): Object {
        42@P.pZero()
    };

    badUndefinedStaticType(): Object {
        (new B)@NotARealClassName.wZero()
    };

    -- bOnly() not on A
    badMethodNotOnStaticType(): Object {
        (new B)@A.bOnly()
    };

    badUndefinedMethod(): Object {
        (new C)@A.noSuchMethodPlease()
    };

    badArityTooFew(): Object {
        (new C)@A.wTwo(1)
    };

    badArityTooMany(): Object {
        (new C)@A.wTwo(1, true, "extra")
    };

    badArgFirst(): Object {
        (new C)@A.wTwo(true, true)
    };

    badArgSecond(): Object {
        (new C)@A.wTwo(1, 1)
    };

    badArgsBoth(): Object {
        (new C)@A.wTwo("s", "t")
    };

    badSelfNotSubtype(): Object {
        self@P.pZero()
    };

    badStringReceiver(): Object {
        "hello"@P.pZero()
    };

    doesSelfTypeWork(): Object {
        (new A)@SELF_TYPE.method()
    };
};

class A {
    wTwo(x: Int, y: Bool): String {
        "A"
    };

    wZero(): Object {
        self
    };

    doesSelfTypeWorkHere(): Object {
        (new B)@SELF_TYPE.wTwo()
    };
};

class B inherits A {
    bOnly(): Int {
        1
    };

    wTwo(x: Int, y: Bool): String {
        "B"
    };

    -- self has static type SELF_TYPE (here: B); B is not a subtype of subclass C
    badSelfToSubtypeStatic(): Object {
        self@C.cZero()
    };
};

class C inherits B {
    cZero(): Object {
        self
    };
};

class P {
    pZero(): Object {
        self
    };
};

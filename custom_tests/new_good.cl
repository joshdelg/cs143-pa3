class Main {
    main(): A {
        new A
    };

    -- dispatch directly on a freshly constructed object
    callOnNew(): Int {
        (new A).id()
    };

    -- SELF_TYPE resolves to receiver, should be B
    callOnNewSelfType(): B {
        (new B).getNewInstance()
    };

    callOnNewSubSelfType(): C {
        (new C).getNewInstance()
    };

    -- dispatch on (new SELF_TYPE)
    spawnAndCall(): SELF_TYPE {
        (new SELF_TYPE).selfReturn()
    };

    selfReturn(): SELF_TYPE { self };
};

class A {
    id(): Int { 0 };
};

class B {
    getNewInstance(): SELF_TYPE {
        new SELF_TYPE
    };
};

class C inherits B {};

-- new on basic classes is legal
class BasicNew {
    newInt(): Int { new Int };
    newString(): String { new String };
    newBool(): Bool { new Bool };
    newIO(): IO { new IO };
    newObject(): Object { new Object };
};
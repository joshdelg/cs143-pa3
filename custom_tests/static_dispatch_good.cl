class Main {
    -- Static dispatch to varying levels of the hierarchy
    main(): Object {
        (new A).foo()
    };

    main2(): Object {
        (new B)@A.foo()
    };

    main3(): Object {
        (new B).foo()
    };

    main4(): Object {
        (new C)@A.foo()
    };
    
    main5(): Object {
        (new C)@B.foo()
    };

    main6(): Object {
        (new C).foo()
    };
};

class A {
    foo() : String {
        "Foo from A"
    };

    fooSelf(): SELF_TYPE {
        self
    };

    -- Static dispatch to SELF_TYPE not allowed
    (* doesSelfTypeWorkHere(): Object {
        (new B)@SELF_TYPE.foo()
    }; *)
};

class B inherits A {
    foo(): String {
        "Foo from B"
    };

    -- Static dispatch off of self
    foo2(): String {
        self@A.foo()
    };

    fooSelf(): SELF_TYPE {
        self
    };

    foo3(): SELF_TYPE {
        fooSelf()
    };

    foo4(): SELF_TYPE {
        self@A.fooSelf()
    };
};

class C inherits B {
    foo(): String {
        "Foo from C"
    };

    foo2(): String {
        self@B.foo()
    };

    foo36(): String {
        self@A.foo()
    };

    fooSelf(): SELF_TYPE {
        self
    };

    foo4(): SELF_TYPE {
        self@B.fooSelf()
    };

    foo5(): SELF_TYPE {
        self@A.fooSelf()
    };
};
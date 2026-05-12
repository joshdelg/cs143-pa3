class Main {
    main(): A {
        new A
    };
};

class A {

};

class B {
    getNewInstance(): SELF_TYPE {
        new SELF_TYPE
    };
};

-- TODO: Do a case involving dispatch
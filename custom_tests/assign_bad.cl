class A {};
class B {};

class Main {
    x: Int <- 0;
    b: B <- new B;

    main(): Object { self };

    -- undeclared
    bad1(): Object {
        undeclaredVar <- 5
    };

    -- wrong type
    bad2(): Object {
        x <- "hello"
    };

    -- wrong type
    bad3(): Object {
        x <- true
    };

    -- wrong type (inheritance)
    bad4(): Object {
        b <- new A
    };

    -- self
    bad5(): Object {
        self <- new Main
    };

    -- type error propogation
    bad6(): Object {
        x <- 1 + true
    };
};

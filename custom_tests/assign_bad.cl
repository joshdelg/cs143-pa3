class A {};
class B {};

class Main {
    x: Int <- 0;
    b: B <- new B;

    main(): Object { self };

    -- Assignment to an undeclared identifier
    bad1(): Object {
        undeclaredVar <- 5
    };

    -- rhs is String; declared type of x is Int; String does not conform to Int
    bad2(): Object {
        x <- "hello"
    };

    -- rhs is Bool; does not conform to Int
    bad3(): Object {
        x <- true
    };

    -- A is not a subtype of B; cannot assign new A to a B variable
    bad4(): Object {
        b <- new A
    };

    -- Assignment to self is forbidden (section 7.2)
    bad5(): Object {
        self <- new Main
    };

    -- rhs type error propagates: (1 + true) has type error; assign result to x
    bad6(): Object {
        x <- 1 + true
    };
};

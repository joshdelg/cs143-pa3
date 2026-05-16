class A {};
class B inherits A {};

class Main {
    x: Int <- 5;
    a: A <- new A;

    main(): Object { self };

    test1(): Int {
        x <- 10
    };

    -- subtype
    test2(): A {
        { a <- new B; a; }
    };

    -- assignment result has the type of the rhs
    test3(): Int {
        let y: Int <- 0 in
            (y <- 5)
    };

    -- chained
    test4(): Int {
        let y: Int <- 0 in
            let z: Int <- 0 in
                y <- (z <- 3)
    };

    -- assignent in let
    test5(): Int {
        let n: Int <- 0 in {
            n <- n + 1;
            n;
        }
    };

    --multiple
    test6(): Int {
        let i: Int <- 0 in {
            i <- i + 1;
            i <- i + 1;
            i;
        }
    };

    -- derived class
    test7(): A {
        let obj: A <- new A in {
            obj <- new B;
            obj;
        }
    };
};

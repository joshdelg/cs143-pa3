class A {};
class B inherits A {};

class Main {
    x: Int <- 5;
    a: A <- new A;

    main(): Object { self };

    -- Same-type assignment: Int <- Int; result type is Int
    test1(): Int {
        x <- 10
    };

    -- Subtype assignment: A <- B (B conforms to A); block needs trailing semicolons
    test2(): A {
        { a <- new B; a; }
    };

    -- Assignment result has the type of the rhs
    test3(): Int {
        let y: Int <- 0 in
            (y <- 5)
    };

    -- Chained assignment (right-associative); both y and z get 3
    test4(): Int {
        let y: Int <- 0 in
            let z: Int <- 0 in
                y <- (z <- 3)
    };

    -- Assignment to a let variable then read it back
    test5(): Int {
        let n: Int <- 0 in {
            n <- n + 1;
            n;
        }
    };

    -- Multiple assignments in a block; last expr gives the block type
    test6(): Int {
        let i: Int <- 0 in {
            i <- i + 1;
            i <- i + 1;
            i;
        }
    };

    -- Assign a more-derived subtype; variable retains declared type A
    test7(): A {
        let obj: A <- new A in {
            obj <- new B;
            obj;
        }
    };
};

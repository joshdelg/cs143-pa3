class Main {
    main(): Bool {
        true
    };

    main2(): Int {
        let x: Int <- 1 in x
    };

    main3(): Int {
        let x: Int in x + 1
    };

    main4(): A {
        let a: A <- new B in a
    };

    main5(): Int {
        let x: Int <- 1, y: Int <- x + 1 in y
    };

    main6(): String {
        let x: Int <- 1, x: String <- "hi" in x
    };

    main7(): String {
        let x: String <- "a" in x
    };

    main7b(): Int {
        x + 1
    };

    main8(): SELF_TYPE {
        let s: SELF_TYPE <- self in s
    };

    main9(): Int {
        let x: Int <- (let y: Int <- 1 in y + 1) in x
    };

    main10(): Int {
        (let x: Int <- 2 in x) + 3
    };

    main11(): Bool {
        (let p: A <- new B in p) = (new B)
    };

    main12(): Int {
        let outer: Int <- 1 in
            let inner: Int <- outer + 1 in
                outer + inner
    };

    x: Int <- 5;
    sumAttr: Int <- let a: Int <- 1, b: Int <- a + 1 in a + b;
};

class A {};

class B inherits A {};

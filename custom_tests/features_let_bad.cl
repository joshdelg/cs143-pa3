class Main {
    main(): Bool {
        true
    };

    bad1(): Int {
        let x: Int <- "hi" in x
    };

    bad2(): Object {
        let s: SELF_TYPE <- new Object in s
    };

    bad2b(): SELF_TYPE {
        let s: SELF_TYPE <- new Main in s
    };

    bad3(): Bool {
        let x: Int <- 1, y: Bool <- 5 in y
    };

    bad4(): Bool {
        (let x: Int <- 1 in (true + 1)) = 2
    };

    bad5(): Int {
        let x: Int <- (1 + true) in x + 2
    };

    bad6(): Int {
        (let y: Int <- 1 in y) + y
    };

    bad7(): Int {
        let x: Int <- (new A).noSuchMethod() in x + 1
    };

    bad7b(): Int {
        let x: Int <- new Bogus in x + 1
    };

    bad8(): Int {
        let x: Int <- (new A)@Bogus.foo() in x + 1
    };

    bad9(): Int {
        let x: Int <- undeclaredId.foo() in x + 1
    };

    bad10(): Int {
        let outer: Int <- 1 in (let inner: String <- "hi" in inner) + outer
    };

    bad13(): Int {
        let x: Int <- 1, x: String <- "hi" in x + 1
    };

    bad14(): Int {
        let self: Int <- 1 in self
    };

    bad14b(): SELF_TYPE {
        let self: SELF_TYPE <- self in self
    };

    badAttr: Int <- let z: Int <- "no" in z;

    bad11(): Int {
        let x: Bogus <- 1 in x
    };

    bad12(): Int {
        let x: AlsoMissing in 0
    };

    badAttr2: Int <- let q: Bogus <- 1 in 0;
};

class A {
    foo(): Int {
        1
    };
};

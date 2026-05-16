class Main {
    main(): Bool {
        1 < 2
    };

    main2(): Bool {
        1 <= 1
    };

    main3(): Bool {
        1 = 2
    };

    main4(): Bool {
        "a" = "b"
    };

    main5(): Bool {
        true = false
    };

    main6(): Bool {
        (new A) = (new A)
    };

    main7(): Bool {
        (new A) = (new B)
    };

    main7a(): Bool {
        (new B).getAInt() < (new B).getBInt()
    };

    main7b(): Bool {
        (new B).getAStr() = (new B).getBStr()
    };

    main7c(): Bool {
        (new B).getAInt() <= 10
    };

    main7d(): Bool {
        not (new B).aLessThanB()
    };

    main7e(): Bool {
        ~(new B).getAInt() <= ~(new B).getBInt()
    };

    main8(): Int {
        ~5
    };

    main9(): Int {
        ~(~(1 + 2))
    };

    main10(): Bool {
        not true
    };

    main11(): Bool {
        not (1 < 2)
    };

    main12(): Bool {
        not (1 + 2 = 3)
    };

    main13(): Bool {
        ~1 <= ~0
    };

    -- non basic types can be compared freely
    main14(): Bool {
        self = self
    };

    main15(): Bool {
        self = new Main
    };

    main16(): Bool {
        self = new SELF_TYPE
    };

    -- void values may be tested for equality
    main17(): Bool {
        let a: A, b: A in a = b
    };

    myBool: Bool <- 1 < 2;
    myBool2: Bool <- 1 <= 2;
    myBool3: Bool <- 1 = 2;
    myBool4: Bool <- not (1 < 2);
    myInt: Int <- ~5;
    myInt2: Int <- ~5 + 1;
};

class A {
    aInt: Int <- 10;
    aStr: String <- "a";
    aBool: Bool <- true;

    getAInt(): Int {
        aInt
    };

    getAStr(): String {
        aStr
    };

    getABool(): Bool {
        aBool
    };
};

class B inherits A {
    bInt: Int <- 20;
    bStr: String <- "b";

    getBInt(): Int {
        bInt
    };

    getBStr(): String {
        bStr
    };

    aLessThanB(): Bool {
        aInt < bInt
    };

    aEqualsB(): Bool {
        aStr = bStr
    };

    notInheritedBool(): Bool {
        not aBool
    };
};

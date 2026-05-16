class Main {
    main(): Object { self };

    -- bool cannot add to int
    bad1(): Int {
        (isvoid self) + 1
    };

    -- bools cannot add
    bad2(): Int {
        (isvoid self) + (isvoid self)
    };

    -- subexpr type error
    bad3(): Bool {
        isvoid (1 + true)
    };

    -- ~ needs int
    bad4(): Int {
        ~(isvoid self)
    };

    -- cannot < bools
    bad5(): Bool {
        (isvoid self) < (isvoid self)
    };

    -- bool != String
    bad6(): String {
        isvoid self
    };
};

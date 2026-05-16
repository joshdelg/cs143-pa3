class Main {
    main(): Object { self };

    -- isvoid returns Bool; Bool cannot be added to Int with +
    bad1(): Int {
        (isvoid self) + 1
    };

    -- Bool + Bool is still a type error (+ requires Int operands)
    bad2(): Int {
        (isvoid self) + (isvoid self)
    };

    -- The subexpression of isvoid has a type error (Bool is not Int)
    bad3(): Bool {
        isvoid (1 + true)
    };

    -- Integer complement (~) requires Int; isvoid returns Bool
    bad4(): Int {
        ~(isvoid self)
    };

    -- Arithmetic comparison (<) requires Int; Bool < Bool is illegal
    bad5(): Bool {
        (isvoid self) < (isvoid self)
    };

    -- isvoid result used where String is expected (method return mismatch)
    bad6(): String {
        isvoid self
    };
};

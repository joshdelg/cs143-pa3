class Main {
    main(): Int {
        true + true
    };

    main2(): Int {
        "Hello" - false
    };

    main3(): Int {
        1 * true
    };

    main4(): Int {
        1 / "cool!"
    };

    main5(): Int {
        1 + 2 * 3 / "bob" * 2 + 9 + false - 1 / 4
    };

    main6(): Int {
        isvoid (1 + 2)
    };

    myInt: Int <- 16;
    myInt2: Int <- 16 + true;
    myInt3: Int <- 15 - 4 + "Bob" * 7 / 4;
    myInt4: Int <- "bobberton" * 3;
    myInt5: Int <- true / 4;

    myIsvoidBad: Int <- isvoid 0;
};
class Main {
    main(): Bool {
        1 < true
    };

    main2(): Bool {
        "a" < "b"
    };

    main3(): Bool {
        true <= false
    };

    main4(): Bool {
        1 = true
    };

    main5(): Bool {
        "a" = 1
    };

    main6(): Bool {
        "a" = false
    };

    main7(): Int {
        ~true
    };

    main8(): Int {
        ~"hello"
    };

    main9(): Bool {
        not 1
    };

    main10(): Bool {
        not "hi"
    };

    main11(): Int {
        ~(1 < 2)
    };

    main12(): Bool {
        not (1 + 2)
    };

    myBool: Bool <- 1 < "x";
    myBool2: Bool <- true <= 3;
    myBool3: Bool <- 1 = "uno";
    myBool4: Bool <- not 5;
    myInt: Int <- ~false;
};

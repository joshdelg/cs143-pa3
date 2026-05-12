class Main {
    main() : Int {0};
    
    -- Exact redef
    main1(): Int {1};
    main1(): Int {1};

    -- Redef with different args
    main2(): Int {1};
    main2(arg1: Int): Int {1};

    -- Redef with diff return type
    main3(): Int {1};
    main3(): Bool {true};

    -- Redef with diff args and return
    main4(): Int {1};
    main4(arg2: Int): Bool {true};

    main5(arg3: Int, arg3: Int): Bool {true};
    main6(arg2: Int, arg2: Bool): Bool {false};

    -- Redef with same type no init
    myInt: Int;
    myInt: Int;

    -- Redef with diff type
    myVar: Bool;
    myVar: Int;
    myVar: String;

    -- Redef with same type different init
    myBool: Bool <- true;
    myBool: Bool <- false;

    -- Redef with diff type diff init
    myVar2: Bool <- true;
    myVar2: String <- "Hello, world!";
};
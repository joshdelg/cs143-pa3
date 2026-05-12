class Main {
    main(): Bool {
        true
    };

    foo(): Int {
        5
    };

    bar(): Int {
        foo()
    };

    fooParams(arg1: Int): Int {
        arg1
    };
    
    barParams(): Int {
        fooParams(5)
    };

    fooParamsMult(arg1: Int, arg2: Bool, arg3: String): String {
        arg3
    };

    barParamsMult(): String {
        fooParamsMult(5, true, "hello world")
    };

    myInt: Int <- 17;
    myBool: Bool <- true;
    myString: String <- "Hello world innit";

    fooParamsVar(arg1: Int, arg2: Bool, arg3: String): String {
        arg3
    };

    barParamsVar(): String {
        fooParamsVar(myInt, myBool, myString)
    };


    fooNew(): A {
        new A
    };

    barNew(): A {
        fooNew()
    };

    fooSelf(): SELF_TYPE {
        new SELF_TYPE
    };

    barSelf(): SELF_TYPE {
        fooSelf()
    };
};

class A {};


class B {
    foo(arg1: Int, arg2: Bool): Int {
        arg1
    };
};

class C inherits B {
    -- Call inherited function
    fooConsumer(): Int {
        foo(1, false)
    };

    -- Call function that is inherited but directly on parent class
    fooConsumer2(): Int {
        (new B).foo(1, true)
    };

    -- Call same named function on different class
    mainFooConsumer(): Int {
        (new Main).foo()
    };
};
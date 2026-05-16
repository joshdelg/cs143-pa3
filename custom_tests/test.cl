class Main {
    b: B;
    main(): Object {
        {
            b <- new B;
            b.set().baz();
        }
    };
 };
-- (a)
class A {
    a_str: String <- "Cool";
    a: SELF_TYPE;
    set() : SELF_TYPE {
        { a <- new A; }
    };
    foo() : String {
        a_str
    };
};
class B inherits A {
    b_str: String <- "Language";
    foo() : String {
        a_str.concat(b_str)
    };
    baz(): String {
        foo().concat(a@A.foo())
    };
};
class A {};
class B inherits A {};
class C inherits B {};

class Main {
    main(): Object { self };

    -- result type is the branch body type
    test1(): A {
        case new B of
            x: B => new A;
        esac
    };

    -- join(B, A) = A
    test2(): A {
        case new A of
            x: B => new B;
            y: A => new A;
        esac
    };

    -- Object as catch all
    test3(): Object {
        case new A of
            x: Int    => 1;
            y: Object => "catch-all";
        esac
    };

    test4(): Int {
        case new A of
            x: Int => x + 1;
            y: A   => 0;
        esac
    };

    test5(): A {
        case new C of
            x: C => new C;
            y: B => new B;
            z: A => new A;
        esac
    };

    -- Use case result in expr
    test6(): Int {
        (case 0 of
            x: Int    => x + 1;
            y: Object => 0;
        esac) + 10
    };

    test7(): Object {
        case new A of
            p: Int    => p;
            q: String => q;
            r: Bool   => r;
            s: A      => s;
        esac
    };

    -- Case on self
    test8(): Object {
        case self of
            x: Main   => x;
            y: Object => y;
        esac
    };

    -- make sure x shadows
    test9(): String {
        case 1 of
            x: String => x;
        esac
    };

    -- use the class' x after the case
    test10(): Int {
        (case 0 of
            x: String => 0;
        esac) + x
    };

    test11(): Int {
        let x: String <- "outer" in
            case 0 of
                x: Int => x + 1;
            esac
    };

    -- join(SELF_TYPE, SELF_TYPE) must stay SELF_TYPE so the method's SELF_TYPE return type conforms.
    test12(): SELF_TYPE {
        case 0 of
            p: Int    => self;
            q: Object => new SELF_TYPE;
        esac
    };

    x: Int <- 5;
};

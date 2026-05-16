class A {};
class B inherits A {};
class C inherits B {};

class Main {
    main(): Object { self };

    -- Single branch; result type is the branch body type
    test1(): A {
        case new B of
            x: B => new A;
        esac
    };

    -- Two branches with different types; join(B, A) = A
    test2(): A {
        case new A of
            x: B => new B;
            y: A => new A;
        esac
    };

    -- Catch-all Object branch; join(Int, String) = Object
    test3(): Object {
        case new A of
            x: Int    => 1;
            y: Object => "catch-all";
        esac
    };

    -- Branch variable is correctly typed and usable in the body
    test4(): Int {
        case new A of
            x: Int => x + 1;
            y: A   => 0;
        esac
    };

    -- Three subtypes; join(C, B, A) = A
    test5(): A {
        case new C of
            x: C => new C;
            y: B => new B;
            z: A => new A;
        esac
    };

    -- Case result used directly in arithmetic
    test6(): Int {
        (case 0 of
            x: Int    => x + 1;
            y: Object => 0;
        esac) + 10
    };

    -- Branches over Int, String, Bool, A; join is Object
    test7(): Object {
        case new A of
            p: Int    => p;
            q: String => q;
            r: Bool   => r;
            s: A      => s;
        esac
    };

    -- Case on self (SELF_TYPE_Main)
    test8(): Object {
        case self of
            x: Main   => x;
            y: Object => y;
        esac
    };
};

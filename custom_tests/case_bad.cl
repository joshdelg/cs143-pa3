class A {};
class B inherits A {};

class Main {
    a: A <- new A;

    main(): Object { self };

    -- declare same type
    bad1(): Object {
        case a of
            y: A => 0;
            z: A => 1;
        esac
    };

    -- Can't use SELF_TYPE for case
    bad2(): Object {
        case a of
            y: SELF_TYPE => y;
        esac
    };

    -- can't redefine self
    bad3(): Object {
        case a of
            self: A => self;
        esac
    };

    -- Undefined class used as a branch type
    bad4(): Object {
        case a of
            y: UndefinedClass => 0;
        esac
    };

    -- Body has type error
    bad5(): Object {
        case a of
            y: A => y + 1;
        esac
    };

    -- Duplicate type appears in third branch among three
    bad6(): Object {
        case a of
            p: B => 0;
            q: A => 1;
            r: B => 2;
        esac
    };

    -- expr failed to typecheck
    bad7(): Object {
        case undeclaredVar of
            x: Object => 0;
        esac
    };

    -- same, but with dispatch
    bad8(): Object {
        case undeclaredVar.someMethod() of
            x: A => 0;
        esac
    };
};

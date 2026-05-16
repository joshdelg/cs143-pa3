class A {};
class B inherits A {};

class Main {
    a: A <- new A;

    main(): Object { self };

    -- Duplicate branch types: both branches declare type A (types must be distinct)
    bad1(): Object {
        case a of
            y: A => 0;
            z: A => 1;
        esac
    };

    -- SELF_TYPE as a branch type (section 4.1 forbids this)
    bad2(): Object {
        case a of
            y: SELF_TYPE => y;
        esac
    };

    -- self as the branch-binding variable (section 7.2 forbids binding self)
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

    -- Body of a branch has a type error (A does not support +)
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
};

class A {};

class Main {
    main(): Object { self };

    -- SELF_TYPE cannot be case type
    bad1(): Object {
        case new A of
            x: SELF_TYPE => x;
        esac
    };

    -- Int doesn't conform to SLEF_TYPE
    bad2(): SELF_TYPE {
        5
    };

    bad3(): SELF_TYPE {
        "hello"
    };

    bad4(): SELF_TYPE {
        new Object
    };

    bad5(): SELF_TYPE {
        new A
    };
};

class A {};

class Main {
    main(): Object { self };

    -- SELF_TYPE as a case branch type (section 4.1 forbids this)
    bad1(): Object {
        case new A of
            x: SELF_TYPE => x;
        esac
    };

    -- Method declares SELF_TYPE return but body is an Int literal
    -- Int is not a subtype of SELF_TYPE_Main (= Main)
    bad2(): SELF_TYPE {
        5
    };

    -- Method declares SELF_TYPE return but body is a String literal
    bad3(): SELF_TYPE {
        "hello"
    };

    -- Method declares SELF_TYPE return but body is a fresh Object
    -- Object is not a subtype of Main
    bad4(): SELF_TYPE {
        new Object
    };

    -- Method declares SELF_TYPE return but body is a fresh A
    -- A is not a subtype of Main
    bad5(): SELF_TYPE {
        new A
    };
};

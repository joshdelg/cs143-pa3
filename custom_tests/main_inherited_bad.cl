-- Section 9: "The main method must be defined in class Main (not inherited
-- from another class)."  Main below only inherits main() from Base;
-- it does not define its own, so this must be an error.

class Base {
    main(): Object {
        self
    };
};

class Main inherits Base {
    -- Intentionally empty: main() is inherited, not defined here
};

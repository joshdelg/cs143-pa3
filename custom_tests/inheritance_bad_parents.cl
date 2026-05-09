-- forbidden / nonexistent parents

class A inherits Int { };       -- can't inherit from Int
class B inherits String { };    -- can't inherit from String
class C inherits Bool { };      -- can't inherit from Bool
class D inherits SELF_TYPE { }; -- can't inherit from SELF_TYPE
class E inherits Nope { };      -- parent doesn't exist

-- redefinition: two classes named F
class F { };
class F inherits IO { };

class Main {
    main() : Object { 0 };
};

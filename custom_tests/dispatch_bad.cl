class Main {
    main(): Bool {
        true
    };

    -- Too few arguments
    badTooFew(): Object {
        fooParams()
    };

    -- Too many arguments
    badTooMany(): Object {
        fooParams(1, 2)
    };

    -- Wrong argument type (Bool where Int required)
    badArgType(): Object {
        fooParams(true)
    };

    -- Mult formal: too few arguments
    badMultArityFew(): Object {
        fooParamsMult(5, true)
    };

    -- Mult formal: wrong type for second (Int instead of Bool)
    badMultArgType(): Object {
        fooParamsMult(5, 42, "s")
    };

    multipleWrongTypes(): Object {
        fooParamsMult("Hello", "Hello", "Hello")
    };

    -- Undefined method on explicit receiver
    badUndefinedOnMain(): Object {
        notAMethod()
    };

    -- dispatch with self as receiver: undefined method
    badSelfUndefined(): Object {
        self.noSuchMethodOnSelf()
    };

    -- Inherited B.foo expects (Int, Bool); too few args
    badInheritedArity(): Object {
        (new C).foo(1)
    };

    -- Inherited: wrong second actual type (Int not Bool)
    badInheritedArgType(): Object {
        (new C).foo(1, 42)
    };

    -- self passed where Int is required
    badSelfAsActual(): Object {
        (new B).foo(self, true)
    };

    -- Primitive receiver: undefined method name
    badPrimitiveReceiver(): Object {
        5.noSuchMethodOnInt()
    };

    -- fooSelf takes no arguments; arity error
    badSelfTypeArity(): Object {
        fooSelf(1)
    };

    -- fooSelf is on Main only; A has no such method
    badSelfTypeWrongReceiver(): Object {
        (new A).fooSelf()
    };

    notSelf(): Int {
        fooSelfSelf()
    };

    passOther(): Object {
        fooTakesSelfType(5)
    };

    passSelf(): Object {
        fooPassedSelfType(self)
    };

    foo(): Int {
        5
    };

    fooParams(arg1: Int): Int {
        arg1
    };

    fooParamsMult(arg1: Int, arg2: Bool, arg3: String): String {
        arg3
    };

    fooSelf(): SELF_TYPE {
        new SELF_TYPE
    };

    fooSelfSelf(): SELF_TYPE {
        self
    };

    fooTakesSelfType(arg1: SELF_TYPE): Bool {
        true
    };

    fooPassedSelfType(arg1: Int): Int {
        arg1
    };

    fooSelf2(): SELF_TYPE {
        5
    };
};

class A {};

class B {
    foo(arg1: Int, arg2: Bool): Int {
        arg1
    };
};

class C inherits B {};

class Main {
    main(): String { "good" };
};

class A inherits Int {
    illegal: Bool <- true;
};

class B {
    legal: Bool <- true;
};
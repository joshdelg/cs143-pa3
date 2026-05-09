-- inheritance graph, classes out of order on purpose
--   * Logger, Main inherit IO
--   * Shape inherits Object
--   * A, B inherit Logger
--   * A1, A2 inherit A
--   * A2a inherits A2
--   * Circle, Polygon inherit Shape
--   * Triangle, Square inherit Polygon

class Square inherits Polygon {
    side : Int <- 1;
};

class A1 inherits A {
    x : Int <- 1;
};

class Logger inherits IO {
    log(s : String) : SELF_TYPE { { out_string(s); self; } };
};

class Polygon inherits Shape {
    sides : Int <- 3;
};

class A2a inherits A2 {
    z : Int <- 3;
};

class Shape inherits Object {
    name : String <- "shape";
};

class A inherits Logger {
    a : Int <- 0;
};

class Triangle inherits Polygon {
    sides : Int <- 3;
};

class B inherits Logger {
    b : Int <- 0;
};

class A2 inherits A {
    y : Int <- 2;
};

class Circle inherits Shape {
    radius : Int <- 1;
};

class Main inherits IO {
    main() : Object { out_string("ok") };
};

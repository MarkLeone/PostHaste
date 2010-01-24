
surface TestPartSeq(varying float f = 0) {
    void foo() {
        float x = f + 1;
        float a = area(P);
    }
    void bar() {
        float y = f + 2;
        float z = y + 3;
        float b = depth(P);
        normal n = calculatenormal(P);
    }
    foo();
    bar();
}

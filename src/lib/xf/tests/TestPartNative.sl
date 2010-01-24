
surface TestPartNative(varying float f = 0) {
    float bar() {
        float y = f + 2;
        return y + 3;
    }

    if (f > 0) {
        float x = f + 1;
        float y = x * 1;
    }
    else {
        float y = f + 2;
        float z = y / 3;
        if (z > 0)
            z = bar();
    }
}

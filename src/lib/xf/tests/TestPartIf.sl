
surface TestPartIf(varying float f = 0) {
    if (f > 0) {
        float x = f + 1;
        float a = area(P);
    }
    else {
        float y = f + 2;
        float z = y + 3;
        float b = depth(P);
        normal n = calculatenormal(P);
    }
}

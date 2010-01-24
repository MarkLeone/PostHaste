
surface TestPartBlock(varying float f = 0) {
    float x = f + 1;
    float a = area(P);
    float y = x + 2;
    float z = y + 3;
    float b = depth(P);
    normal n = calculatenormal(P);
}

surface TestCgShader1(float a = 1; output varying float b = 0;)
{
    b += a;
    print(a);
    print(b);
    print(P);
    Ci = Cs;
    b = a + 1;
    float f = P[0];
    print(f);
}

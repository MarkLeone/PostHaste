
surface TestLiveIllum6(varying float a = 1;
                       output varying float x = 0) 
{
    // Illuminance args don't kill references after loop
    illuminance (P, "send:light:foo", a, "light:bar", x) {
        Ci += Cl;
    }
    print(x);
}

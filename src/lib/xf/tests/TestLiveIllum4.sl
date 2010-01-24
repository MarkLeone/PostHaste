
surface TestLiveIllum4(varying float a = 1;
                       output varying float x = 0) 
{
    // Test illuminance args.
    illuminance (P, "send:light:foo", a, "light:bar", x) {
        Ci += Cl;
    }
}


surface TestLiveIllum3(varying float a = 1;
                       output varying float x = 0) 
{
    // Loop-local variables aren't live outside
    illuminance (P) {
        color c = Cl;
        if (c[0] > .1)
            Ci += Cl;
    }
}

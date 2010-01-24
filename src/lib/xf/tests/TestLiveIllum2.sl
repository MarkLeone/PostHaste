
surface TestLiveIllum2(output varying float x = 0) 
{
    // The loop body doesn't kill variables that are live after.
    illuminance (P) {
        Ci += Cl;
        x = 0;
    }
    print(x);
}

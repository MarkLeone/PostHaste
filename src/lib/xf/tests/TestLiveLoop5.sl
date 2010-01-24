
surface TestLiveLoop5(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // Condition variable is not killed by loop body.
    while (x < a) {
        x = a;
        y += 1;
    }
    print(y);
}

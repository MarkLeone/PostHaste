
surface TestLiveLoop6(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // y is live in iterate statement, killed in loop body.
    for (; x < a; x += y) {
        y = 1;
    }
}

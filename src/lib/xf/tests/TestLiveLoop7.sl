
surface TestLiveLoop7(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // y is live in body, not killed by iterate statement.
    for (; x < a; y = 1) {
        x += y;
    }
}

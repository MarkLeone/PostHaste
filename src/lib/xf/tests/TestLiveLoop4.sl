
surface TestLiveLoop4(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // x is not killed by iterate stmt.
    for (; a < n; x = 0) {
        print(x);
        if (x == 0)
            break;
    }
}

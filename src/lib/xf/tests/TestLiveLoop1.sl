
surface TestLiveLoop1(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // live: a, n, y
    float i = n;
    while (i > 2) {
        y += a;
        i -= 1;
    }
    print(y);
}

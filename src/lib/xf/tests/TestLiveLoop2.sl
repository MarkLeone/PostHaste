
surface TestLiveLoop2(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // y is not killed by the loop
    float i = n;
    while (i > 2) {
        y = 0;
        i -= 1;
    }
    print(y);
}


surface TestLive4(varying float a = 1, b = 2;
                  output varying float x = 0, y = 0;) 
{
    // X is live because it is not initialized on all paths.
    float x;
    if (a > 0)
        x = 1;
    if (b > 0)
        print(x);
}

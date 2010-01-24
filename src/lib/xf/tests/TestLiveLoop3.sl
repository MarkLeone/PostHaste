
surface TestLiveLoop3(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // x is killed by the cond stmt (unused by new shader compiler, however).
    for (x = 0; x < n; x += a) {
    }
    print(x);
}


surface TestLiveLoop9(varying float a = 1, n = 5;
                      output varying float x = 0, y = 0;) 
{
    // Live in the loop body, but killed by the condition statement.
    for (; (x=1) < y; y -= 1) {
        print(x);
    }
}


surface TestLive1(varying float a = 1, b = 2;
                  output varying float x = 0) 
{
    // Live: a, b.  x is killed by the assignments in both branches.
    print(a);
    if (b > 0)
        x = 1;
    else
        x = 2;
    print(x);
    print(b);
}


surface TestLive2(varying float a = 1, b = 2;
                  output varying float x = 0, y = 0;) 
{
    // x and y are live, because they're killed in only one branch.
    if (a > 0)
        x = 1;
    if (b > 0)
        x = 2;
    else
        y = 1;
    print(x);
    print(y);
}

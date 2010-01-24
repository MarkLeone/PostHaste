
surface TestLive3(varying float a = 1, b = 2;
                  output varying float x = 0, y = 0;) 
{
    // C and D are both initialized, and therefore killed.
    float c = a+1;
    float d = b+1;
    if (a > 0)
        print(c);
}

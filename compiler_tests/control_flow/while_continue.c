int f()
{
    int x;
    x=20;
    int y;
    y = 0;
    while(x > 10){
        if (x == 11){
            x=x-1;
            continue;
        }
        x=x-1;
        y=y+1;
    }
    return y;
}

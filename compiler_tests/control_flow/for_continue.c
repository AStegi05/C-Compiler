int f()
{
    int x;
    int y;
    y=0;
    for(x=0; x<10; x=x+1){
        if(x == 9){
            continue;
        }
        y=y-1;
    }
    return y;
}

int f(int x, int y);

int main()
{
    if( (f(20,2)!=5) ) return 1;
    if( (f(-20,2)!=-5) ) return 1;
    if( (f(21,2)!=5) ) return 1;
    if( (f(1,1)!=0) ) return 1;
    if( (f(0,2)!=0) ) return 1;
    return 0;
}

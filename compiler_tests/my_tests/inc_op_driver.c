int f(int x);

int main()
{
    if( (f(0)!=1) ) return 1;
    if( (f(1)!=2) ) return 1;
    if( (f(100)!=101) ) return 1;
    if( (f(10)!=11) ) return 1;
    if( (f(-11)!=-10) ) return 1;
    return 0;
}

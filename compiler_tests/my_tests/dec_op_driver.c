int f(int x);

int main()
{
    if( (f(0)!=-1) ) return 1;
    if( (f(1)!=0) ) return 1;
    if( (f(100)!=99) ) return 1;
    if( (f(10)!=9) ) return 1;
    if( (f(-11)!=-12) ) return 1;
    return 0;
}

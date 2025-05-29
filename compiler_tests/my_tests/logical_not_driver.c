
int f(int x);

int main()
{
    if( (f(0)!=1) ) return 1;
    if( (f(10)!=0) ) return 1;
    if( (f(-10)!=0) ) return 1;
    if( (f(1)!=0) ) return 1;
    if( (f(34)!=0) ) return 1;
    return 0;
}

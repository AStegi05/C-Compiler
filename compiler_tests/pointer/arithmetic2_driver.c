int f();

int main()
{
    int x[3];
    x[0] = 42;
    x[1] = 13;
    x[2] = 99;
    return !(f(x) == 13);
}

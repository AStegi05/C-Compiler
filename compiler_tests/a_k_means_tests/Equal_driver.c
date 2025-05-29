struct Point {
    float x, y;
};

float Equal(struct Point a, struct Point b);

int main()
{
    struct Point a = {1, 4};
    struct Point b = {1, 6};
    return !(Equal(a, b))== 1 && (Equal(a, b) == 0);
}

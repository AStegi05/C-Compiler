struct Point {
    float x, y;
};

float fabsf(float x) {
    return x < 0 ? -x : x;
}
int Equal(struct Point a, struct Point b) {
    float EPS = 0.000001;
    return (fabsf(a.x - b.x) < EPS) && (fabsf(a.y - b.y) < EPS);
}

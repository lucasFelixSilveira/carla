struct test {
    char a;
    char b;
};

int main() {
    struct test x;
    void* a = &x.a;
    *((char*)a) = 7;
    void* b = &x.b;
    *((char*)b) = 3;
    int y = *((char*)a);
    int z = *((char*)b);

    int w = y + z;
    int v = w - 10;
    if( v != 0 ) goto zero;
    // ALERT (MORGANA TEM FUNCAO EXTRA, N VALE IMPLEMENTAR)
    zero: {}
}

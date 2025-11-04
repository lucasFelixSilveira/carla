#include <stdint.h>
#if not(defined(limited_hardware))
#include <array>
#include <vector>
#endif

struct Morgana {
#   if defined(limited_hardware)
    Morgana() = default;
#   else
    char **argv;
    Morgana(char **argv) : argv(argv) {}
#   endif

    int32_t main() {
        return 0;
    }
};

#if defined(limited_hardware)
int main() {
    Morgana run;
    return run.main();
}
#else
int main(int argc, char **argv) {
    Morgana run(argv);
    return run.main();
}
#endif

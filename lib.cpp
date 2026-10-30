int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    int result = n;
    for (int i = (n-1); i > 1; i--) {
        result *= i;
    }
    return result;
}
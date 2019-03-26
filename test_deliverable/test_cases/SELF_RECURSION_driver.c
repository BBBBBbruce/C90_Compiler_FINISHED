int self_recursion();
int main() { return !( 5 == self_recursion(2) ); }

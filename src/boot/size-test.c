/* Print sizes of various constants on N64 screen */

/* Does boot code pass arguments that we can use as args to main? */

int main() {
	long long foo = 0x1010101010101010LL;
	long long bar = 0x0202020202020202LL;
	long long asdf = foo + bar;
	printf("char: %d\n", sizeof(char));// 1
	printf("short: %d\n", sizeof(short));//2
	printf("int: %d\n", sizeof(int));//4
	printf("long: %d\n", sizeof(long));//4
	printf("long long: %d\n", sizeof(long long));//8
	printf("float: %d\n", sizeof(float));//4
	printf("double: %d\n", sizeof(double));//8
	printf("long double: %d\n", sizeof(long double));//8
	printf("ptr: %d\n", sizeof(void*));//4
}
/* If main returns, perform a reset */

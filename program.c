#include <stdio.h>

struct point
{
    int x;
    int y;
};

int modulo(int x, int mod)
{
    return (x % mod + mod) % mod;
}
int modular_inverse(int x, int mod)
{
    int i = 0;
    for (i = 0; i < mod; i++)
    {
        if (modulo((x * i), mod) == 1)
        {
            return i;
        }
    }
}
void addition(struct point *result, struct point *P, struct point *Q, int modulus)
{
    int s, y_diff, x_diff, inverse;
    x_diff = Q->x - P->x;
    inverse = modular_inverse(x_diff, modulus);
    s = modulo((inverse * (Q->y - P->y)), modulus);
    result->x = modulo(((s * s) - P->x - Q->x), modulus);
    result->y = modulo(((s * (P->x - result->x)) - P->y), modulus);
}

void doubling(struct point *result, struct point *P, int alpha, int modulus)
{
    int s, inverse;
    inverse = modular_inverse(2 * P->y, modulus);
    s = modulo((inverse * (3 * P->x * P->x + alpha)), modulus);
    result->x = modulo(((s * s) - P->x - P->x), modulus);
    result->y = modulo(((s * (P->x - result->x)) - P->y), modulus);
}

void integer_multiplication(struct point *result, struct point *P, int multiplier, int alpha, int modulus)
{
    int total_bits = (sizeof(int) * 8) - 1;
    int bit = 0;
    int i = 0, temp = 0;
    struct point intermediate;
    for (i = total_bits; i >= 0; i--)
    {
        temp = (multiplier >> i);
        bit = temp & 1;
        if (bit)
        {
            break;
        }
    }
    intermediate = *P;
    i--;
    for (; i >= 0; i--)
    {
        doubling(result, &intermediate, alpha, modulus);
        intermediate = *result;
        bit = (multiplier >> i) & 1;
        if (bit)
        {
            addition(result, &intermediate, P, modulus);
            intermediate = *result;
        }
    }
}

int main(int argc, char *argv[])
{
    // Let the elliptic curve be y^2 = x^3 - x + 16 mod 29
    int prime = 29, alpha = -1, beta = 16;
    int secret_key = 17, private_key_b = 23;
    struct point G = {5, 7};
    struct point A = {0, 0}, B = {0, 0}, Sa = {0, 0}, temp = {0, 0};
    int message1 = 28, message2 = 25, decrypted_message1, decrypted_message2;
    int CM1, CM2;
    integer_multiplication(&A, &G, secret_key, alpha, prime);
    printf("Public key is: E: y^2 = x^3 + %dx + %d mod %d, G: (%d, %d), A: (%d, %d)\n", alpha, beta, prime, G.x, G.y, A.x, A.y);
    integer_multiplication(&B, &G, private_key_b, alpha, prime);
    integer_multiplication(&temp, &G, secret_key, alpha, prime);
    integer_multiplication(&Sa, &temp, private_key_b, alpha, prime);
    CM1 = modulo(Sa.x * message1, prime);
    CM2 = modulo(Sa.y * message2, prime);
    printf("Encrypted message is: ((%d, %d), %d, %d)\n", B.x, B.y, CM1, CM2);

    integer_multiplication(&temp, &B, secret_key, alpha, prime);

    decrypted_message1 = modulo(modular_inverse(temp.x, prime) * CM1, prime);
    decrypted_message2 = modulo(modular_inverse(temp.y, prime) * CM2, prime);
    printf("Decrypted message is: %d, %d\n", decrypted_message1, decrypted_message2);
}
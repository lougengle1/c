static int g_a =2;
const int g_c_b = 3;


void memory_dump(void *ptr, int len) {
    int i;
    printf("%p\n", ptr);
    for (i = 0; i < len; i++) {
        if (i % 8 == 0 && i != 0)
            printf(" ");
        if (i % 16 == 0 && i != 0)
            printf("\n");
        printf("%02x ", *((unsigned char *)ptr + i));
    }
    printf("\n");
}


int main()
{
    static int m_a =3;
    char *c = "1234";
    char *e = "1234";
    const int m_c_b =3;

    memory_dump(&g_a,sizeof(g_a));
    memory_dump(&m_a,sizeof(m_a));

    memory_dump(&g_c_b,sizeof(g_c_b));
    memory_dump(&m_c_b,sizeof(m_c_b));

    memory_dump(c,sizeof(c));
    memory_dump(e,sizeof(e));

    return 0;
}
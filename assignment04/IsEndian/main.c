unsigned int is_endian(unsigned int *val);

unsigned int is_little_endian(unsigned int *val)
{
   
    char *c = (char*)&val;
    
    if(*c)
    {
        // return 0 is Little Endian
        return 0;
    }
    
    // return 1 is Big Endian
    return 1;
}

int main()
{
    unsigned int digit = 1998;
    is_little_endian(&digit);
    return 0;
}

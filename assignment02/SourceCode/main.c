
int counter = 0x0;

int main() {
  int *p_int = (int *)0x20000000;
  
  ++(*p_int);
  ++(*p_int);
  ++(*p_int);
  counter ++;
  return 0;
}


/*unsigned counter = 0;

int main()
{
//int counter = 0;
//unsigned int counter = 0;
//unsigned counter = 0;

counter ++;
counter ++;
counter ++;
counter ++;
counter ++;
counter ++;
counter ++;
return 0;
}*/

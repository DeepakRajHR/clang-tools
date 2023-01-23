void one_complex(void) {
  return;
}

void two_complex(void) {
  if (true) return;
  return;
}

void three_complex(void) {
  if (true)
    ;
  if (true)
    ;
}

#define MACRO_FOUR_COMPLEX(ins) \
  if (true)                     \
    if (true)                   \
      if (true) ins

void four_complex(int param) {
  if (true)
    ;
  if (true)
    ;
  if (true)
    ;
}

void four_complex_macro(void) {
  MACRO_FOUR_COMPLEX(
      ;)
}

void five_complex(void) {
  if (true && true)
    ;
  if (true)
    ;
  if (true)
    ;
}

int ten_complex(void) {
  short i, j, k;

  i = 1;
  if (i < 2)                    /*predicate node 1*/
  {
    i = 2;
    for (j = 1; j < i; j++)     /*predicate node 2*/
      k = j;
    if (j < i)                  /*predicate node 3*/
    {
      j = i;
      k = j;
    }
  }
  i = j;
  one_complex();
  switch (i)                    /*this won't count*/
  {
    case 1:                     /*predicate node 4*/
    case 2:                     /*predicate node 5*/
      j = k; break;

    case 3:                     /*pridicate node 6*/
    case 4:                     /*predicate node 7*/
      if (i == j)               /*predicate node 8*/
        i = k;

    default:                    /*predicate node 9*/
      k = 1; break;
  }
  i = j;
  while (j < i)                 /*predicate node 10*/
    j++;
  return (1);
}

// EOF

#include <string.h>
#include <stdio.h>

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/custom.h>
#include <caml/callback.h>


void
dump_margin(int margin)
{
  while (margin-- > 0)
    printf(" ");
}

void
dump_caml_value(value v, size_t margin)
{
  size_t	size;
  size_t	i;


  if (Is_long(v))
    {
      dump_margin(margin);
      printf("unboxed (%ld) : %ld\n", v, Long_val(v));
      return;
    }

  dump_margin(margin);
  printf("boxed value with size = %zu\n", size = Wosize_val(v));

  switch (Tag_val(v))
    {
      case Closure_tag:
	dump_margin(margin);
	printf("| Closure with %zu free variable(s)\n", size - 1);

	dump_margin(margin + 2);
	printf("| Pointer: %p\n", Code_val(v));

	//i = 1;
	//while (i++ < size)
	//  dump_caml_value(Field(v, i), margin + 2);
	break;
      case String_tag:
	dump_margin(margin);
	printf("| String (%s): %s\n", (char *) v, String_val(v));
	break;
      case Double_tag:
	dump_margin(margin);
	printf("| Double: %g\n", Double_val(v));
	break;
      case Double_array_tag:
	dump_margin(margin);
	printf("| Double array: [");

	i = 0;
	while (i++ < size / Double_wosize)
	  {
	    if (i - 1 == size / Double_wosize)
	      printf("%g", Double_field(v, i - 1));
	    else
	      printf("%g, ", Double_field(v, i - 1));
	  }

	printf("]\n");
	break;
      case Abstract_tag:
	dump_margin(margin);
	printf("| Abstract type\n");
	break;
      default:
	if (Tag_val(v) >= No_scan_tag)
	  {
	    dump_margin(margin);
	    printf("| Unknown tag\n");
	  }
	else
	  {
	    dump_margin(margin);
	    printf("| Structured block (tag = %d):\n", Tag_val(v));

	    i = 0;
	    while (i++ < size)
	      dump_caml_value(Field(v, i - 1), margin + 2);
	  }
    }
}

/* val dump : 'a -> unit */

CAMLprim value
rainbow_dump(value a)
{
  CAMLparam1(a);

  dump_caml_value(a, 0);

  CAMLreturn(Val_unit);
}

#include <libefi.h>

static VOID put(const CHAR16 *string)
{
  ST->ConOut->OutputString(ST->ConOut, (CHAR16 *) string);
}

static VOID puti(INTN number)
{
  CHAR16 string[21] = {0};
  itos(string, number);
  put(string);
}

static VOID print_hex(UINT64 number)
{
  CHAR16 str[17] = {0};
  uitoh(str, number);
  put(str);
}

VOID puts(const CHAR16 *string)
{
  put(string);
  put(L"\n\r");
}

VOID putchar(CHAR16 character)
{
  CHAR16 string[2] = {character, L'\0'};

  ST->ConOut->OutputString(ST->ConOut, string);
}

INTN printf(const CHAR16 *format, ...)
{
  va_list vl;

  va_start(vl, format);
  while(*format)
  {
    if(*format == L'%')
    {
      format++;
      switch(*format)
      {
        case L'%':
          putchar(L'%');
          break;
        case L'i':
          puti(va_arg(vl, INTN));
          break;
        case L'c':
          putchar((CHAR16) va_arg(vl, UINTN));
          break;
        case L's':
          put(va_arg(vl, CHAR16 *));
          break;
        case L'X':
        case L'x':
          print_hex(va_arg(vl, UINT64));
          break;
      }
    }
    else
      putchar(*format);

    format++;
  }

  va_end(vl);
  
  return 0;
}

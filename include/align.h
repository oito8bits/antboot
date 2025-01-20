#ifndef _ALIGN_INCLUDE_
#define _ALIGN_INCLUDE_

#define IS_ALIGN(value, align) ((value) & ((align) - 1))
#define ALIGNUP(value, align) ((value) + ((align) - 1) & ~((align) - 1))

#endif

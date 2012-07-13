#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#ifdef NDEBUG
#define VIRTUAL
#else
#define VIRTUAL virtual
#endif

#define null 0

#ifdef __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE inline
#endif

#endif //DEFINITIONS_H

#ifndef BITS_H_
#define BITS_H_ 1

#define bit_get(flags,bit)	(flags & (bit))
#define bit_set(flags,bit)	(flags |= (bit))
#define bit_off(flags,bit)	(flags &= ~(bit))

#define BIT_CLR bit_off
#define BIT_ISSET bit_get
#define BIT_SET bit_set

#endif /* BITS_H_ */

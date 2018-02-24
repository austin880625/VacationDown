#include <stdlib.h>
#include <stdint.h>
size_t convfrom_utf8(unsigned char *to, unsigned char *from, size_t len);
size_t convto_utf8(unsigned char *to, unsigned char *from, size_t len);
size_t getutf8ch(unsigned char *tmp, uint16_t c);

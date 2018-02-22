#include "utf8.h"
#include <stdint.h>
size_t convfrom_utf8(unsigned char *to, unsigned char *from, size_t len)
{
	int cur = 0;
	for(size_t i=0; i<len; ){
		if(from[i] <= 127){
			to[cur] = from[i]; to[cur+1] = 0x00;
			i++;
		}
		else if(from[i] <= 223){
			to[cur] = from[i+1]&0x3f; to[cur+1] = from[i]&0x1f;
			i+=2;
		}
		else if(from[i] <= 239){
			to[cur] = ((from[i+1]&0x03)<<6)|(from[i+2]&0x3f);
			to[cur+1] = ((from[i]&0x0f)<<4)|((from[i+1]&0x3C)>>2);
			i+=3;
		}
		cur+=2;
	}
	return cur;
}
size_t convto_utf8(unsigned char *to, unsigned char *from, size_t len)
{
	uint16_t c;
	int cur = 0;
	for(size_t i=0; i<len; i+=2){
		c = ((uint16_t)from[i+1]<<8)|((uint16_t)from[i]);
		if(c <= 0x007f){
			to[cur]=c&0xff;
			cur++;
		}
		else if(c <= 0x07ff){
			to[cur] = 0xc0|(c>>6);
			to[cur+1] = 0x08|(c&0x3f);
			cur+=2;
		}
		else{
			to[cur] = 0xd0|(c>>12);
			to[cur+1] = 0x80|((c>>6)&0x3f);
			to[cur+2] = 0x80|(c&0x3f);
			cur+=3;
		}
	}
	return cur;
}

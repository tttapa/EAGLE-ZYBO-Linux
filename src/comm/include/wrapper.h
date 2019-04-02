#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int wmemf(uint64_t offset, float value);
int wmem(uint64_t offset, uint32_t value);
int rmem(uint64_t offset);
int flight_mode();
int attitude(int arg);
int attitude(int arg);
int flight_mode();
void output(float throttle, float rotx, float roty, float rotz);
int curr_z();

#ifdef __cplusplus
}
#endif

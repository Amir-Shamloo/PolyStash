#include "b3d_file.h"

CFStringRef StringToCF(const cineware::String &in);
void	CFToString(const CFStringRef in, cineware::String *out);
cineware::String CFToString(CFStringRef in);

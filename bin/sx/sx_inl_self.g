%{
#include <string.h>
#include <inl.h>
#include "sx.h"
%}
struct inl_self_tab;
%%
90
180
270
aka
atf
c
d
end
f
fake
form
forms
g
h
images
k
l
listdef
lit
lref
n
pcun
s
sign
t
ucode
uname
useq
v
z
%%
int
sx_inl_self_wrapper(const char *n)
{
  return sx_inl_self(n, strlen(n)) ? 1 : 0;
}

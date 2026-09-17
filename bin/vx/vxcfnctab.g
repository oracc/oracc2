%{
#include <stdlib.h>
#include <string.h>
#include <cat.h>
#include "vx.h"
%}
struct vxcfnctab;
%%
composite,	(vx_catf_fnc*)vxc_composite
transliteration,(vx_catf_fnc*)vxc_transliteration
protocols,	(vx_catf_fnc*)vxc_protocols
protocol,	(vx_catf_fnc*)vxc_protocol
object,		(vx_catf_fnc*)vxc_obj_sur
surface,	(vx_catf_fnc*)vxc_obj_sur
column,		(vx_catf_fnc*)vxc_column
l,			(vx_catf_fnc*)vxc_l
xcl,		(vx_catf_fnc*)vxc_xcl

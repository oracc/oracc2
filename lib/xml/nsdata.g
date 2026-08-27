%{
#include <oraccsys.h>
#include <xml.h>
%}
struct nsdata;
%%
"http://oracc.org/ns/atf/1.0","atf",NS_ATF
"http://oracc.org/ns/cbd/1.0","c",NS_CBD
"http://oracc.org/ns/xff/1.0","xff",NS_XFF
"http://oracc.org/ns/gdl/1.0","g",NS_GDL
"http://oracc.org/ns/sl/1.0","s",NS_SL
"http://oracc.org/ns/xtf/1.0","x",NS_XTF
"http://oracc.org/ns/xcl/1.0","xcl",NS_XCL
"http://oracc.org/ns/xmd/1.0","m",NS_XMD
"http://oracc.org/ns/xtr/1.0","xtr",NS_XTR
"http://oracc.org/ns/cfy/1.0","cfy",NS_CFY
"http://oracc.org/ns/inl/1.0","i",NS_INL
"http://oracc.org/ns/bib/1.0","b",NS_BIB
"http://www.w3.org/1999/xhtml","xh",NS_HTM
"http://www.w3.org/XML/1998/namespace","xml",NS_XML
atf,"http://oracc.org/ns/atf/1.0",NS_ATF
c,"http://oracc.org/ns/cbd/1.0",NS_CBD
xff,"http://oracc.org/ns/xff/1.0",NS_XFF
g,"http://oracc.org/ns/gdl/1.0",NS_GDL
s,"http://oracc.org/ns/sl/1.0",NS_SL
x,"http://oracc.org/ns/xtf/1.0",NS_XTF
xcl,"http://oracc.org/ns/xcl/1.0",NS_XCL
xtr,"http://oracc.org/ns/xtr/1.0",NS_XTR
m,"http://oracc.org/ns/xmd/1.0",NS_XMD
cfy,"http://oracc.org/ns/cfy/1.0",NS_CFY
inl,"http://oracc.org/ns/inl/1.0",NS_INL
b,"http://oracc.org/ns/bib/1.0",NS_BIB
xh,"http://www.w3.org/1999/xhtml",NS_HTM
xml,"http://www.w3.org/XML/1998/namespace",NS_XML
%%
void
nsdata_set_key_data(NSdata **np)
{
  int siz=sizeof(nsdata_wl)/sizeof(struct nsdata);
  int i;
  for (i = 0; i < siz; ++i)
    {
      if (!strchr(nsdata_wl[i].name, ':'))
        np[nsdata_wl[i].code] = &nsdata_wl[i];
    }
}

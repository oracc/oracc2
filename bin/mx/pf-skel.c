#include <oraccsys.h>
#include "pf_lib.h"

#define P0(a) 		fprintf(ipf->output_fp,"%s",(a))
#define P1(f,a)		fprintf(ipf->output_fp,(f),(a))
#define P2(f,a1,a2)	fprintf(ipf->output_fp,(f),(a1),(a2))
#define P3(f,a1,a2,a3)	fprintf(ipf->output_fp,(f),(a1),(a2),(a3))

static void
skel_before_outers(void)
{
}

static void
skel_each_outer (void)
{
}

static void
skel_between_outers(void)
{
}

static void
skel_after_outers(void)
{
}

static void
skel_each_text(void)
{
}

static void
skel_before_block(void)
{
}

static void
skel_after_block(void)
{
}

static void
skel_before_sources(void)
{
}

static void
skel_between_sources(void)
{
}

static void
skel_after_sources(void)
{
}

static void
skel_before_composite(void)
{
}

static void
skel_after_composite(void)
{
}

static void
skel_each_composite_column(void)
{
}

static void
skel_between_composite_columns(void)
{
}

static void
skel_before_reconstructed(void)
{
}

static void
skel_after_reconstructed(void)
{
}

static void
skel_each_reconstructed_column(void)
{
}

static void
skel_between_reconstructed_columns(void)
{
}

static void
skel_before_source(void)
{
}

static void
skel_after_source(void)
{
}

static void
skel_each_source_column(void)
{
}

static void
skel_between_source_columns(void)
{
}

static void
skel_before_notes(void)
{
}

static void
skel_each_note(void)
{
}

static void
skel_after_notes(void)
{
}

Process_functions pf_skel = {
NULL, NULL,
skel_before_outers,
skel_each_outer,
skel_between_outers,
skel_after_outers,
skel_each_text,
skel_before_block,
skel_after_block,
skel_before_sources,
skel_between_sources,
skel_after_sources,
skel_before_composite,
skel_after_composite,
skel_each_composite_column,
skel_between_composite_columns,
skel_before_reconstructed,
skel_after_reconstructed,
skel_each_reconstructed_column,
skel_between_reconstructed_columns,
skel_before_source,
skel_after_source,
skel_each_source_column,
skel_between_source_columns,
skel_before_notes,
skel_each_note,
skel_after_notes,
};

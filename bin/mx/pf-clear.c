#include <oraccsys.h>
#include "pf_lib.h"

static void
clear_before_outers(void)
{
}

static void
clear_each_outer (void)
{
}

static void
clear_between_outers(void)
{
}

static void
clear_after_outers(void)
{
}

static void
clear_each_text(void)
{
}

static void
clear_before_block(void)
{
}

static void
clear_after_block(void)
{
}

static void
clear_before_sources(void)
{
}

static void
clear_between_sources(void)
{
}

static void
clear_after_sources(void)
{
}

static void
clear_before_composite(void)
{
}

static void
clear_after_composite(void)
{
}

static void
clear_each_composite_column(void)
{
  iterate_composite_column->done = FALSE;
}

static void
clear_between_composite_columns(void)
{
}

static void
clear_before_reconstructed(void)
{
}

static void
clear_after_reconstructed(void)
{
}

static void
clear_each_reconstructed_column(void)
{
  iterate_reconstructed_column->done = FALSE;
}

static void
clear_between_reconstructed_columns(void)
{
}

static void
clear_before_source(void)
{
}

static void
clear_after_source(void)
{
}

static void
clear_each_source_column(void)
{
  iterate_source_column->done = FALSE;
}

static void
clear_between_source_columns(void)
{
}

static void
clear_before_notes(void)
{
}

static void
clear_each_note(void)
{
}

static void
clear_after_notes(void)
{
}

Process_functions pf_clear = {
NULL, NULL,
clear_before_outers,
clear_each_outer,
clear_between_outers,
clear_after_outers,
clear_each_text,
clear_before_block,
clear_after_block,
clear_before_sources,
clear_between_sources,
clear_after_sources,
clear_before_composite,
clear_after_composite,
clear_each_composite_column,
clear_between_composite_columns,
clear_before_reconstructed,
clear_after_reconstructed,
clear_each_reconstructed_column,
clear_between_reconstructed_columns,
clear_before_source,
clear_after_source,
clear_each_source_column,
clear_between_source_columns,
clear_before_notes,
clear_each_note,
clear_after_notes,
};

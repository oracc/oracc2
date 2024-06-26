const char *p4controls = 
"<x>\n"
"  <div id=\"p4itemNav\" class=\"p4items\" data-ityp=\"@@pui_item_index_typ()@@\" data-inth=\"@@pui_item_index_nth()@@\" data-imax=\"@@pui_active_items()@@\">\n"
"    <p class=\"bg-dk\">\n"
"\n"
"      \n"
"      <img id=\"p4ToggleMeta\" class=\"p4icon\" src=\"/img/p3-full.png\"\n"
"	   alt=\"Toggle metadata pane\" title=\"Toggle metadata pane\" onclick=\"toggle_md()\"/>\n"
"      \n"
"      <img id=\"p4PageItem\" class=\"p4icon p4spread\" src=\"/img/p3-pageitem.png\"\n"
"	   alt=\"Toggle Page/Item view\" title=\"Return to Page view\"\n"
"	   onclick=\"toggle_pi()\"/>\n"
"\n"
"      <!--Item-->\n"
"      <img id=\"p4ItemPrev\" class=\"p4icon p4spread\" src=\"/img/p3-backward.png\"\n"
"	   alt=\"Item back\" title=\"Item back\"\n"
"	   onclick=\"act_item_prev()\"/>\n"
"      \n"
"      <span id=\"p4ItemView\"> [@@pui_item_index()@@ of @@pui_active_items()@@] </span>\n"
"      \n"
"      <img id=\"p4ItemNext\" class=\"p4icon p4spread\" src=\"/img/p3-forward.png\"\n"
"	   alt=\"Item forward\" title=\"Item forward\"\n"
"	   onclick=\"act_item_next()\"/>\n"
"\n"
"      <span id=\"p4SrchResults\" class=\"p4srchresults\" data-tmax=\"@@pui_tmax()@@\" data-rmax=\"@@pui_srch_results()@@\">\n"
"	@@pui_tmax_texts()@@ [@@pui_srch_results_matches()@@]\n"
"      </span>\n"
"\n"
"      <span id=\"p4Translations\">\n"
"	@@pui_select_trans()@@\n"
"	<input id=\"p4Transonly\" type=\"checkbox\" onchange=\"toggle_to()\"\n"
"	       value=\"@@pui_transonly()@@\"/>&#xa0;only\n"
"      </span>\n"
"\n"
"    </p>\n"
"  </div>\n"
"\n"
"  <div id=\"p4PageNav\" class=\"p4pages\" data-page=\"@@pui_current_page()@@\" data-pmax=\"@@pui_active_pages()@@\" data-imax=\"@@pui_active_items()@@\">\n"
"    <p class=\"p3toccenter bg-dk\">\n"
"\n"
"      <button id=\"p4Zoom\" class=\"p4spread\" onclick=\"act_zoom('0')\">All Results</button>\n"
"\n"
"      <img id=\"p4PagePrev\" class=\"p4icon p4spread\" src=\"/img/p3-backward.png\"\n"
"	   alt=\"Page back\" title=\"Page back\"\n"
"	   onclick=\"act_prev()\"/>\n"
"      \n"
"      <!--Page-->\n"
"      <input id=\"p4Pageset\" type=\"text\" onchange=\"act_page()\" size=\"3\" value=\"@@pui_current_page()@@\"/>\n"
"\n"
"      <img id=\"p4PageNext\" class=\"p4icon p4spread\" src=\"/img/p3-forward.png\"\n"
"	   alt=\"Page forward\" title=\"Page forward\"\n"
"	   onclick=\"act_next()\"/>\n"
"\n"
"      @@pui_active_items()@@ @@pui_item_label()@@\n"
"      /\n"
"      @@pui_active_pages()@@ @@pui_page_label()@@\n"
"\n"
"      <span id=\"p4SrchResultpages\" class=\"p4srchresults\">[@@pui_srch_results()@@ matches]</span>\n"
"\n"
"      <span id=\"p4CeType\" class=\"cetype\">@@pui_select_ce()@@</span>\n"
"    </p>\n"
"  </div>\n"
"\n"
"</x>\n"
"\n"
;

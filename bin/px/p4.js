function qs_append(q,c) {
    if (q.length == 0) {
	q = '?';
    } else {
	q = q+'&';
    }
    return q+c;
}

function item_oid(i) {
    return i.startsWith("o") || i.startsWith("x");
}

function itemLocation() {
    let pager = getPager();
    let item = pager.getAttribute("data-item");
    if (item && item.length) {
	let proj = pager.getAttribute("data-proj");
	let glos = pager.getAttribute("data-glos");
	let list = pager.getAttribute("data-list");
	let perm = pager.getAttribute("data-sort");
	let loc = "/"+proj+"/";
	if (glos && glos.length > 0 && item_oid(item)) {
	    loc = loc+glos+"/"+item;
	} else {
	    loc = loc+item;
	}
	let qs = '';
	if (list && list !== "outlined.lst") {
	    qs = qs_append(qs, 'list='+list);
	}
	if (perm && perm !== "123") {
	    qs = qs_append(qs, 'perm='+perm);
	}

	loc = loc+qs;

	alert("itemLocation="+loc);
	window.location = loc;
    } else {
	alert('itemLocation called but item is NULL');
    }
}

function pageLocation() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let glos = pager.getAttribute("data-glos");
    let loc = '';    
    if (glos && glos.length > 0) {
	let i = 0;
	let qs = '';
	let zoom = pager.getAttribute("data-zoom");
	if (zoom) {
	    qs = 'zoom='+zoom;
	}
	let page = pager.getAttribute("data-page");
	if (page) {
	    if (qs.length) {
		qs = qs+'&';
	    }
	    qs = qs+'page='+page;
	}
	let gxis = pager.getAttribute("data-gxis");
	if (gxis) {
	    qs = qs+'&xis='+gxis;
	}
	loc = '/'+proj+'/'+glos+'?'+qs;
    } else {
	let list = pager.getAttribute("data-list");
	let perm = pager.getAttribute("data-sort");
	let zoom = pager.getAttribute("data-zoom");
	let page = pager.getAttribute("data-page");
	let bkmk = pager.getAttribute("data-bkmk");
	let qs = '';
	if (list && list !== "outlined.lst") {
	    qs = qs_append(qs, 'list='+list);
	}
	if (perm && perm !== "123") {
	    qs = qs_append(qs, 'perm='+perm);
	}
	if (zoom && zoom !== "0") {
	    qs = qs_append(qs, 'zoom='+zoom);
	}
	if (page && page !== "1") {
	    qs = qs_append(qs, 'page='+page);
	}

	loc = '/'+proj;
	if (qs.length == 0) { // test before bkmk on purpose
	    loc = loc+"/pager";
	}

	if (bkmk) {
	    qs = qs_append(qs, 'bkmk='+bkmk);
	}

	loc = loc+qs;
	
    }
    alert("pageLocation="+loc);
    window.location=loc;
}

function resetPager() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let loc = '/'+proj+'/pager';
    // alert("loc="+loc);
    window.location=loc;
}

function getPager() {
    return document.getElementById("p4Pager");
}

function act_sorter(dors) {
    let perm = document.getElementById(dors).value;
    let pager = getPager();
    let currperm = pager.getAttribute('data-sort');
    if (perm !== currperm) {
	pager.setAttribute('data-sort', perm);
	pageLocation();
    }
}

function act_sorter_default() {
    act_sorter('p4OSdefault');
}

function act_sorter_special() {
    act_sorter('p4OSspecial');
}

function act_article(item) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    itemLocation();
}

function act_item(item) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    itemLocation();
}

function act_item_next() {
    let pager = getPager();
    let inext = pager.getAttribute("data-next");
    if (inext) {
	pager.setAttribute("data-item", inext);
	itemLocation();
    } else {
	alert('You are already at the last item.');
    }
}

function act_item_prev() {
    let pager = getPager();
    let iprev = pager.getAttribute("data-prev");
    if (iprev) {
	pager.setAttribute("data-item", iprev);
	itemLocation();
    } else {
	alert('You are already at the first item.');
    }
}

function act_next() {
    let pager = getPager();
    let page = pager.getAttribute("data-page")/1;
    let pmax = pager.getAttribute("data-pmax")/1;
    if (page < pmax) {
	++page;
	pager.setAttribute("data-page", page);
	pageLocation();
    } else {
	alert('You are already at the last page (page='+page+'; pmax='+pmax+')');
    }
}

function act_page() {
    let newpage = document.getElementById("p4Pageset").value/1;
    // alert('p4Pageset value='+newpage);
    let pager = getPager();
    let pmax = pager.getAttribute("data-pmax")/1;
    if (newpage > 0 && newpage <= pmax) {
	pager.setAttribute("data-page", newpage);
	pageLocation();
    } else {
	alert('The page value must be greater than zero and no bigger than '+pmax);
    }
}

function act_prev() {
    let pager = getPager();
    let page = pager.getAttribute("data-page")/1;
    if (page > 1) {
	--page;
	pager.setAttribute("data-page", page);
	pageLocation();
    } else {
	alert('You are already at the first page');
    }
}

function act_zoom(z) {
    let pager = getPager();
    let glos = pager.getAttribute("data-glos");
    pager.removeAttribute("data-bkmk");
    if (glos) {
	if (z === '0') {
	    pager.setAttribute("data-page", '1');
	    pager.setAttribute("data-zoom", "entry_ids");
	    pager.removeAttribute("data-item");
	    pageLocation();
	} else {
	    act_letter(z);
	}
    } else {
	let currzoom = pager.getAttribute("data-zoom")/1;
	let nextzoom = z/1;
	if (currzoom != nextzoom) {
	    pager.setAttribute("data-page", '1');
	    pager.setAttribute("data-zoom", z);
	    pager.removeAttribute("data-item");
	    pageLocation();
	}
    }
}
	
function act_letter(thisletter) {
    let pager = getPager();
    let currletter = pager.getAttribute("data-zoom");
    // alert('thisletter='+thisletter+'; currletter='+currletter);
    if (thisletter !== currletter) {
	pager.setAttribute("data-page", '1');
	pager.setAttribute("data-zoom", thisletter);
	pager.removeAttribute("data-item");
	pageLocation();
    }
}

// toggles implemented with hide attribute or class-dependent display=none
function toggle_dd() {
    document.getElementById("p4Menu").classList.toggle('hide');
}
function toggle_md() {
    document.getElementById("p4XtfMeta").classList.toggle('hide');
}
function toggle_pi() {
    let pager = getPager();
    let item = pager.getAttribute("data-item");
    if (item) {
	pager.setAttribute("data-bkmk", item);
	pager.removeAttribute("data-item");
	pageLocation();
    } else {
	pager.setAttribute("data-item", pager.getAttribute("data-bkmk"));
	itemLocation();
    }
}

function toggle_to() {
    document.getElementById("p4XtfData").classList.toggle('transonly');
}

// translation setting and toggling
function act_translation() {
    let trans = document.getElementById("p4TransSelect").value;
    if (trans == "none") {
	document.getElementById("p4XtfData").classList.toggle('textonly');
    } else {
	if (document.getElementById("p4XtfData").classList.contains('textonly')) {
	    document.getElementById("p4XtfData").classList.toggle('textonly');
	}
	let pager = getPager();
	let currlang = pager.getAttribute("data-lang");
	if (currlang !== trans) {
	    pager.setAttribute("data-lang", trans);
	    itemLocation();
	}
    }
}

// P4 CBD

function gloart(id) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    itemLocation();    
}

// P4 quick search implementation is URL-based

function qs_add(pager, qs, datum) {
    let val = pager.getAttribute("data-"+datum);
    if (val && val.length != 0) {
	let amp = (qs.length ? '&' : '');
	return qs+amp+datum+'='+val;
    } else {
	return qs;
    }
}

// act_srch is called when the search box changes or is activated and
// only passes a URL to wx if the srch is new
function act_srch() {
    let srch=document.getElementById('p4SrchBox').value;
    if (srch.length == 0) {
	return;
    } else {
	let pager = getPager();
	let proj = pager.getAttribute('data-proj');
	let glos = pager.getAttribute('data-glos');
	let qury = pager.getAttribute('data-qury'); /* if current page came from srch
						       the srch terms are stored in data-qury */
	if (qury !== srch) {
	    if (glos) { glos = '/'+glos; } else { glos = '' }
	    let srch_url='/'+proj+glos+'?srch='+encodeURIComponent(srch);
	    // alert('srch_url='+srch_url);
	    window.location = srch_url;
	} else {
	    alert('Srch '+srch+' has not changed');
	}
    }
}

// P3 continuing JS

function popup(url,windowName,height,width,screenX,screenY) {
  popupWindow = window.open(url,windowName,
 "dependent=yes,"
 +"directories=no,"
 +"height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"location=yes,menubar=no,resizable=yes,scrollbars=yes,titlebar=no,toolbar=no");
  popupWindow.focus();
  return popupWindow;
}

// In P3 sigfixer added (empty) lang and proj arguments to the pop1sig
// call but in P4 we have data-proj and are ignoring the old (and by
// default empty) lang arg for now
function pop1sig(sig) {
    var bio = '\u2623'; // force encoding always to be utf8
    var esig = encodeURIComponent(bio+sig);
    let pager = getPager();
    let proj = pager.getAttribute('data-proj');
    var url = '/'+proj+'/sig?'+esig;
    popup(url,'cbdarticle',400,600,0,0);
}

function showblock(project,item,blockid) {
    popup('/'+project+'/'+item+'/score?'+blockid,400,600,700,50);
}

function showcdli(PQ) {
    var url = 'http://cdli.ucla.edu/'+PQ;
    popup(url,'exemplarWindow',700,800,100,350);
}

function showexemplar(project,PQ,lineid,fragid) {
    showexemplar2(project,'',PQ,lineid,fragid);
}

function showexemplar2(project,lang,PQ,lineid,fragid) {
    var url = '/'+project+'/'+PQ+'/html';
    if (lineid.length > 0) {
	url = url+'?'+lineid;
	if (fragid.length > 0) {
	    url = url+','+fragid;
	}
    }
    var win = window;
    popup(url,'exemplarWindow',500,700,100,350);
    /* win.focus(); */
}


function getPager() {
    return document.getElementById("p4Pager");
}

function getCurrentPage() {
    return document.getElementById("p4PageNav").getAttribute("data-page");
}

function setCurrentPage(p) {
    document.getElementById("p4PageNav").setAttribute('data-page', p);
}

function getCurrentPmax() {
    return document.getElementById("p4PageNav").getAttribute("data-pmax");
}

function loc_append(l,c) {
    if (l.length != 0) {
	l = l+'/';
    }
    return l+c;
}

function qs_append(q,c) {
    if (q.length == 0) {
	q = '?';
    } else {
	q = q+'&';
    }
    return q+c;
}

function perm_is_default(p) {
    return p.length == 1
	|| (p.length == 2 && p === "12")
	|| (p.length == 3 && p === "123");
}

function item_oid(i) {
    return i.startsWith("o") || i.startsWith("x");
}

function itemLocation() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let gent = pager.getAttribute("data-gent");
    let item = pager.getAttribute("data-item");
    let gxis = pager.getAttribute("data-gxis");
    let loc = "/"+proj;
    if (gent || gxis) {
	loc = loc_append(loc, pager.getAttribute("data-glos"));
	if (gent) {
	    loc = loc_append(loc, gent);
	}
	let qs = '';
	if (gxis) {
	    qs = qs_append(qs, 'xis='+gxis);
	    let perm = pager.getAttribute("data-sort");
	    if (perm && !perm_is_default(perm)) {
		qs = qs_append(qs, 'sort='+perm);
	    }
	    if (item) {
		qs = qs_append(qs, 'ref='+item);
	    }
	}

	loc = loc+qs;

	// alert("gent:itemLocation="+loc);
	window.location = loc;

    } else if (item) {
	let list = pager.getAttribute("data-list");
	let srch = pager.getAttribute("data-srch");
	let perm = pager.getAttribute("data-sort");
	let lang = pager.getAttribute("data-lang");
	let loc = "/"+proj;
	loc = loc_append(loc,item);

	let qs = '';
	if (srch) {
	    qs = qs_append(qs, 'srch='+srch);
	} else {
	    if (list && list !== "outlined.lst") {
		qs = qs_append(qs, 'list='+list);
	    }
	}
	if (perm && !perm_is_default(perm)) {
	    qs = qs_append(qs, 'sort='+perm);
	}
	if (lang && lang.length > 0 && lang !== "(null)") {
	    qs = qs_append(qs, 'lang='+lang);
	}

	loc = loc+qs;

	// alert("item:itemLocation="+loc);
	window.location = loc;
    } else {
	alert('itemLocation called but item is NULL');
    }
}

function pageLocation() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let glos = pager.getAttribute("data-glos");
    let gxis = pager.getAttribute("data-gxis");
    let srch = pager.getAttribute("data-srch");
    let page = getCurrentPage();
    let loc = '';    
    // alert('pageLocation; glos='+glos);
    if (glos && !gxis) {
	let i = 0;
	let qs = '';
	let zoom = pager.getAttribute("data-glet");
	if (zoom) {
	    qs = qs_append(qs, 'zoom='+zoom);
	}
	if (page) {
	    qs = qs_append(qs, 'page='+page);
	}	
	loc = '/'+proj+'/'+glos;
	if (qs.length > 0) {
	    loc = loc+qs;
	}
    } else {
	// alert('pageLocation non-glos/gxis');
	let list = pager.getAttribute("data-list");
	let perm = pager.getAttribute("data-sort");
	let zoom = pager.getAttribute("data-zoom");
	let bkmk = pager.getAttribute("data-bkmk");
	let glet = pager.getAttribute("data-glet");
	let qs = '';

	if (list && list !== "outlined.lst") {
	    qs = qs_append(qs, 'list='+list);
	} else {
	    if (gxis) {
		qs = qs_append(qs, 'xis='+gxis);
	    } else if (srch) {
		qs = qs_append(qs, 'srch='+srch);
	    }
	}

	if (perm && !perm_is_default(perm)) {
	    qs = qs_append(qs, 'sort='+perm);
	}
	if (zoom && zoom !== "0") {
	    qs = qs_append(qs, 'zoom='+zoom);
//	} else if (glet) {			// this should be irrelevant if glos is not set
//	    qs = qs_append(qs, 'zoom='+glet);
	}
	if (page && page !== "1") {
	    qs = qs_append(qs, 'page='+page);
	}

	loc = '/'+proj;
	
	if (glos) {
	    loc = '/'+proj+'/'+glos;
	} else {
	    if (qs.length == 0) { // test before bkmk on purpose
		loc = loc+"/pager";
	    }
	}

	if (bkmk) {
	    qs = qs_append(qs, 'bkmk='+bkmk);
	}

	loc = loc+qs;
	
    }
    // alert("pageLocation="+loc);
    window.location=loc;
}

function resetPager() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let loc = '/'+proj+'/pager';
    // alert("loc="+loc);
    window.location=loc;
}

function act_sorter() {
    let perm = document.getElementById("p4MenuSelect").value;
    let pager = getPager();
    let currperm = pager.getAttribute('data-sort');
    // alert('sort='+perm+'; currsort='+currperm);
    if (perm != currperm) {
	// alert('changing sort order');
	setCurrentPage('1');
	pager.setAttribute('data-sort', perm);
	pager.setAttribute('data-zoom', '0');
	pager.removeAttribute('data-item');
	pageLocation();
    }
}

function act_article(item) {
    let pager = getPager();
    pager.setAttribute("data-gent", item);
    itemLocation();
}

function act_item(item) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    itemLocation();
}

function act_iref(evt) {
    let pager = getPager();
    pager.setAttribute("data-item", evt.target.getAttribute('data-iref');
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
    let page = getCurrentPage()/1;
    let pmax = getCurrentPmax()/1;
    if (page < pmax) {
	++page;
	setCurrentPage(page);
	pageLocation();
    } else {
	alert('You are already at the last page (page='+page+'; pmax='+pmax+')');
    }
}

function act_page() {
    let newpage = document.getElementById("p4Pageset").value/1;
    // alert('p4Pageset value='+newpage);
    let pager = getPager();
    let pmax = getCurrentPmax()/1;
    if (newpage > 0 && newpage <= pmax) {
	setCurrentPage(newpage);
	pageLocation();
    } else {
	alert('The page value must be greater than zero and no bigger than '+pmax);
    }
}

function act_prev() {
    let pager = getPager();
    let page = getCurrentPage()/1;
    if (page > 1) {
	--page;
	setCurrentPage(page);
	pageLocation();
    } else {
	alert('You are already at the first page');
    }
}

function act_zoom(z) {
    let pager = getPager();
    let glos = pager.getAttribute("data-glos");
    let gxis = pager.getAttribute("data-gxis");
    // alert('act_zoom glos='+glos+'; gxis='+gxis);
    if (glos && !gxis) {
	if (z === '0') {
	    setCurrentPage('1');
	    pager.setAttribute("data-glet", "entry_ids");
	    pager.removeAttribute("data-zoom");
	    pager.removeAttribute("data-item");
	    pageLocation();
	} else {
	    act_letter(z);
	}
    } else {
	let currzoom = pager.getAttribute("data-zoom")/1;
	let nextzoom = z/1;
	if (currzoom != nextzoom) {
	    setCurrentPage('1');
	    pager.setAttribute("data-zoom", z);
	    pager.removeAttribute("data-item");
	    pager.removeAttribute("data-bkmk");
	    pageLocation();
	}
    }
}
	
function act_letter(thisletter) {
    let pager = getPager();
    let currletter = pager.getAttribute("data-zoom");
    // alert('thisletter='+thisletter+'; currletter='+currletter);
    if (thisletter !== currletter) {
	setCurrentPage('1');
	pager.setAttribute("data-glet", thisletter);
	pager.removeAttribute("data-zoom");
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
// This control has changed--it now appears only in item view and it only toggles back to page view
function toggle_pi() {
    let pager = getPager();
    let item = pager.getAttribute("data-item");
    if (item) {
	pager.setAttribute("data-bkmk", item);
	pager.removeAttribute("data-item");
	pageLocation();
//    } else {
//	pager.setAttribute("data-item", pager.getAttribute("data-bkmk"));
//	itemLocation();
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
	    pager.removeAttribute('data-item');
	    let srch_url='/'+proj+glos+'?q='+encodeURIComponent(srch);
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

/* Notes implementation from notes.js, not from p3.js which seems never to have been used */
function
xshowNote(e,nid) {
  if (!e)
    e = event;
  note=document.getElementById(nid);
  note.style.left=e.clientX-100;
  note.style.top=e.clientY;
  // alert(nid+' is at '+e.clientX+' x '+e.clientY);
  // makes note element visible
  note.style.visibility='visible';
  return 1;
}
function
hideNote(e,nid) {
  if (!e)
    e = event;
  note=document.getElementById(nid);
  // makes note element visible
  note.style.visibility='hidden';
  return 1;
}
function
showNote(e,nid,trans) {
    if (!e) var e = event;
    var posx = 0;
    var posy = 0;
    var safari = 0;
    var ua = navigator.userAgent.toLowerCase(); 
    if (ua.indexOf('safari')!=-1){
	safari = 1;
    }

    // alert('showNote called with nid='+nid);
    
    if (!e) e = event;
    if (e.clientX || e.clientY) {
	posx = e.clientX - window.pageXOffset;
	if (e.clientY > 0) {
	    posy = e.clientY;
	}
    } else if (e.pageX || e.pageY) {
	posx = e.pageX;
	posy = e.pageY;
    }
    
    // posx and posy contain the mouse position relative to the document
    // Do something with this information
    note=document.getElementById(nid);


//    alert('Event is at xy '+posx+'x'+posy);
    
    jQsel = "#"+nid;
    nidh = $(jQsel).height();
    nidw = $(jQsel).width();
    // alert('Note '+jQsel+' is '+nidh+'hx'+nidw+'w');
    
    p3rh = $("#p4Content").height() - 100;
    p3rw = $("#p4Content").width();
    // alert('Container is '+p3rh+'hx'+p3rw+'w');

    if (note.style.visibility != "visible") {
	if (trans) {
	    note.style.left = (posx-200)+'px';
	} else {
	    note.style.left = posx+'px';
	}

	spaceup = posy;
	spacedn = p3rh-posy;

	notehalf = nidh/2;

	if (notehalf <= spaceup && notehalf <= spacedn) {
//	    alert('middle');
	    note.style.top = (posy/2)+'px';
	} else if (notehalf >= spaceup) {
//	    alert('below');
	    note.style.top = posy;
	} else {
//	    alert('above');
	    //	    note.style.top = posy+'px';
	    note.style.top = (posy-nidh)+'px';
	}
	
	note.style.visibility='visible';
	note.style.zIndex = 3;
    }
    return 1;
}

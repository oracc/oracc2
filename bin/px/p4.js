function updateLocation() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let glos = pager.getAttribute("data-glos");
    let loc = '';
    if (glos) {
	let i = 0;
	let qs = '';
	let zoom = pager.getAttribute("data-zoom");
	if (zoom) {
	    qs = 'zoom='+zoom;
	}
	let item = pager.getAttribute("data-item");
	if (item) {
	    if (qs.length) {
		qs = qs+'&';
	    }
	    qs = qs+'item='+item;
	} else {
	    let page = pager.getAttribute("data-page");
	    if (page) {
		if (qs.length) {
		    qs = qs+'&';
		}
		qs = qs+'page='+page;
	    }
	}
	loc = '/'+proj+'/'+glos+'?'+qs;
    } else {
	let list = pager.getAttribute("data-list");
	let perm = pager.getAttribute("data-sort");
	let item = pager.getAttribute("data-item");
	if (item) {
            loc = '/'+proj+'?list='+list+'&sort='+perm+'&item='+item;
	} else {
	    let zoom = pager.getAttribute("data-zoom");
	    let page = pager.getAttribute("data-page");
	    let bkmk = pager.getAttribute("data-bkmk");
	    loc = '/'+proj+'?list='+list+'&sort='+perm+'&zoom='+zoom+'&page='+page;
	    if (bkmk) {
		loc = loc+'&bkmk='+bkmk;
	    }
	}
    }
    alert("updateLocation="+loc);
    window.location=loc;
}

function resetPager() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let list = pager.getAttribute("data-list");
    let loc = '/'+proj+'?list='+list;
    alert("loc="+loc);
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
	updateLocation();
    }
}

function act_sorter_default() {
    act_sorter('p4OSdefault');
}

function act_sorter_special() {
    act_sorter('p4OSspecial');
}

function act_item(item) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    updateLocation();
}

function act_item_next() {
    let pager = getPager();
    let inext = pager.getAttribute("data-next");
    if (inext) {
	pager.setAttribute("data-item", inext);
	updateLocation();
    } else {
	alert('You are already at the last item.');
    }
}

function act_item_prev() {
    let pager = getPager();
    let iprev = pager.getAttribute("data-prev");
    if (iprev) {
	pager.setAttribute("data-item", iprev);
	updateLocation();
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
	updateLocation();
    } else {
	alert('You are already at the last page (page='+page+'; pmax='+pmax+')');
    }
}

function act_page() {
    let newpage = document.getElementById("p4Pageset").value/1;
    alert('p4Pageset value='+newpage);
    let pager = getPager();
    let pmax = pager.getAttribute("data-pmax")/1;
    if (newpage > 0 && newpage <= pmax) {
	pager.setAttribute("data-page", newpage);
	updateLocation();
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
	updateLocation();
    } else {
	alert('You are already at the first page');
    }
}

function act_zoom(z) {
    let pager = getPager();
    let currzoom = pager.getAttribute("data-zoom")/1;
    let nextzoom = z/1;
    if (currzoom != nextzoom) {
	pager.setAttribute("data-page", '1');
	pager.setAttribute("data-zoom", z);
	pager.removeAttribute("data-item");
	updateLocation();
    }
}
	
function act_letter(thisletter) {
    let pager = getPager();
    let currletter = pager.getAttribute("data-zoom");
    alert('thisletter='+thisletter+'; currletter='+currletter);
    if (thisletter !== currletter) {
	pager.setAttribute("data-page", '1');
	pager.setAttribute("data-zoom", thisletter);
	pager.removeAttribute("data-item");
	updateLocation();
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
    } else {
	pager.setAttribute("data-item", pager.getAttribute("data-bkmk"));
    }
    updateLocation();
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
	    updateLocation();
	}
    }
}

// P4 CBD

function gloart(id) {
    let pager = getPager();
    pager.setAttribute("data-item", item);
    updateLocation();    
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

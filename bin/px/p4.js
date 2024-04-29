function updateLocation() {
    let pager = getPager();
    let proj = pager.getAttribute("data-proj");
    let list = pager.getAttribute("data-list");
    let zoom = pager.getAttribute("data-zoom");
    let page = pager.getAttribute("data-page");
    let loc = '/'+proj+'?list='+list+'&zoom='+zoom+'&page='+page;
    alert("loc="+loc);
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
	updateLocation();
    }
}

function toggle_dd() {
    document.getElementById("p4Menu").classList.toggle('auto');
}
function toggle_md() {
    alert('toggle_md=>'+document.getElementById("p4Header").classList.toggle('menu'));
}

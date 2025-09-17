function getCuneify() {
    return document.getElementById("p4Cuneify");
}

function cuneify(c) {
    //let c = getCuneify();
    let currf = c.getAttribute("data-cfy-fnt")
    let f = '--ofs-'+currf;
    let currffs = c.getAttribute("data-cfy-ffs");
    let ffs = '--ffs-'+currf;
    let m = c.getAttribute("data-cfy-mag")+'%';
    let s = '--ofs-'+c.getAttribute("data-cfy-scr");

    let b = document.getElementById("p4CuneifyBar");
    //alert('b='+b);
    if (b) {
	let p = b.firstChild;
	//alert('p='+p);
	let bb = p.children;
	for (let i = 0; i < bb.length; i++) {
	    bb[i].classList.remove("cfy-curr");
	    let thisf = bb[i].getAttribute("data-font");
	    if (thisf === currf) {
		//alert('switch on '+thisf);
		bb[i].classList.toggle("cfy-curr");
	    }
	}
    }
    
    //alert('f='+f+'; m='+m+'; s='+s);

    var r = document.querySelector(':root');
    r.style.setProperty('--ofs-font', 'var('+f+')');
    r.style.setProperty('--ofs-ffs', 'var('+ffs+')');
    r.style.setProperty('--ofs-mag', m);
    r.style.setProperty('--ofs-script', 'var('+s+')');
}

function getData(evt) {
    let n = evt.target;
    while (!n.hasAttribute('onclick')) {
	n = n.parentElement;
    }
    return n;
}

function cuneify_reset(evt) {
    let e = getData(evt);
    let f = e.getAttribute('data-font');
    let c = getCuneify();
    c.setAttribute('data-cfy-fnt', f);
    cuneify();
}

// Create a function for getting a variable value
//function myFunction_get() {
  // Get the styles (properties and values) for the root
// var rs = getComputedStyle(r);
  // Alert the value of the --blue variable
//  alert("The value of --blue is: " + rs.getPropertyValue('--blue'));
//}


function getCuneify() {
    return document.getElementById("p4Cuneify");
}

function cuneify() {
    let c = getCuneify();
    let f = '--ofs-'+c.getAttribute("data-cfy-fnt");
    let m = c.getAttribute("data-cfy-mag")+'%';
    let s = '--ofs-'+c.getAttribute("data-cfy-scr");

    alert('f='+f+'; m='+m+'; s='+s);
    
    var r = document.querySelector(':root');
    r.style.setProperty('--ofs-font', 'var('+f+')');
    r.style.setProperty('--ofs-mag', m);
    r.style.setProperty('--ofs-script', 'var('+s+')');
}

function cuneify_reset(f) {
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

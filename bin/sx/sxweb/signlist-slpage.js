var projpath='/@@project@@/signlist/';

function cmap (chr) {
    var c = chr.charCodeAt(0);
    if (chr == '|') {
	return "";
    } else if (c < 128) {
	return chr;
    } else if (c > 0x2079 && c < 0x208A) {
	// alert(chr+' code '+c);
	return String.fromCharCode(c - 0x2050);
    } else {
	if (chr == 'š') {
	    return 'c';
	} else if (chr == 'ŋ') {
	    return 'j';
	} else if (chr == '×') {
	    return "*";
	} else if (chr == 'ₓ') {
	    return "x";
	} else if (chr == 'ṣ') {
	    return 'f';
	} else if (chr == 'ś') {
	    return 'ss';
	} else if (chr == 'ṭ') {
	    return 'v';
	} else if (chr == 'ḫ') {
	    return 'h';
	} else if (chr == 'ʾ') {
	    return "'";
	} else {
	    return chr;
	}
    }
}

function kmap (str) {
    let lc = str.toLowerCase();
    let lc1 = lc.replace(/s,/, "f");
    let lc2 = lc1.replace(/t,/, "v");
    var res = '';
    for (var c of lc2) {
	res += cmap(c);
    }
    return res;
}

function slpage () {
    var form=document.getElementById('sl');
    // alert('form='+form);
    var key=document.getElementById('k').value;
    // alert('key='+key);
    var ctdr = '';
    if (key.startsWith('*')) {
	ctdr = '-ctd';
	key = key.substring(1);
	alert('key='+key+'; ctdr='+ctdr);
    } else if (key.endsWith('*')) {
	ctdr = '-ctr';
	key = key.substring(0, key.length - 1);
	alert('key='+key+'; ctdr='+ctdr);
    }
    var mkey = kmap(key);
    // alert(mkey);
    // alert('mkey='+mkey);
    
    var oc = mkey.charCodeAt(0) - "a".charCodeAt(0);
    if (oc >= 0 && oc <= 26) {
        var ao = aa[oc];
        // alert(ao);                                                                                                               
        var oid=ao[mkey];
        if (oid) {
            alert('oid='+oid);                                                                                                   
            if (oid.startsWith("selpages/")) {
                window.location=projpath+oid+'.html';
	    } else if (ctdr.length > 0) {
		oid = oid.replace(/\w+/, "");
		alert('oid-sans-letter='+oid);
                window.location=projpath+'selpages'+oid+ctdr+'.html';		
	    } else {
                window.location=projpath+oid+'/index.html';
            }
        } else {
            if (key !== mkey) {
                key += ' (> '+mkey+')';
            }
            alert('Nothing found for '+key+': if this is a corpus signlist that value may not be known in this corpus');
        }
    } else {
        alert('Invalid lookup table key "'+key+'": keys must begin with a letter');
    }
}

function xslpage () {
    var form=document.getElementById('sl');
    // alert('form='+form);
    var key=document.getElementById('k').value;
    // alert('key='+key);
    var mkey = kmap(key);
    // alert('mkey='+mkey);
    var oid=x[mkey];
    if (oid) {
	// alert('oid='+oid);
	if (oid.startsWith("selpages/")) {
	    window.location=projpath+oid+'.html';
	} else {
	    window.location=projpath+oid+'/index.html';
	}
    } else {
	if (key !== mkey) {
	    key += ' (> '+mkey+')';
	}
	alert('Nothing found for '+key+': if this is a corpus signlist that value may not be known in this corpus');
    }
}

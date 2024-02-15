var projpath='/@@project@@/signlist/';

function cmap (chr) {
    var c = chr.charCodeAt(0);
    if (c < 128) {
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
    var res = '';
    for (var c of lc) {
	res += cmap(c);
    }
    return res;
}

function slpage () {
    var form=document.getElementById('sl');
    // alert('form='+form);
    var key=document.getElementById('k').value;
    // alert('key='+key);
    var mkey = kmap(key);
    // alert('mkey='+mkey);
    var oid=x[mkey];
    if (oid) {
	// alert('oid='+oid);
	window.location=projpath+oid+'/index.html';
    } else {
	if (key !== mkey) {
	    key += ' (> '+mkey+')';
	}
	alert('Nothing found for '+key+': if this is a corpus signlist that value may not be known in this corpus');
    }
}

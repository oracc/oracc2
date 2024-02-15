var projpath='/@@project@@/signlist/';

function slpage () {
    var form=document.getElementById('sl');
    // alert('form='+form);
    var key=document.getElementById('k').value;
    // alert('key='+key);
    var oid=x[key];
    if (oid) {
	// alert('oid='+oid);
	window.location=projpath+oid+'/index.html';
    } else {
	alert('Nothing found for '+key+': if this is a corpus signlist that value may not be known in this corpus');
    }
}

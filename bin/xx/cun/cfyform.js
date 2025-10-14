document.addEventListener('DOMContentLoaded', function() {
    const cfyForm = document.getElementById('cfyForm');
    const xdata = document.getElementById('xdata');

    cfyForm.addEventListener('submit', function(event) {
        event.preventDefault(); // Prevent default form submission

        const originalText = xdata.value;
        const encodedText = encodeURIComponent(originalText);
	
        // Create a hidden input field to send the encoded data
	let hiddenInput = document.getElementById('data');
	if (!hiddenInput) {
            hiddenInput = document.createElement('input');
            hiddenInput.type = 'hidden';
	    hiddenInput.id = 'data';
            hiddenInput.name = 'data'; // Name for the server-side
	}
        hiddenInput.value = encodedText;

	// alert('data='+hiddenInput.value);

        // Append the hidden input to the form
        cfyForm.appendChild(hiddenInput);

        // Submit the form with the encoded data
        cfyForm.submit();
    });
});

function cfyKey() {
    let cKey='cfy-'
	+document.getElementById('cK-font').value+'-'
	+document.getElementById('cK-feat').value+'-'
	+document.getElementById('cK-mag').value+'-'
	+document.getElementById('cK-script').value+'-'
	+document.getElementById('cK-asl').value;
    let hiddenKey = document.getElementById('key');
    if (!hiddenKey) {
            hiddenKey = document.createElement('input');
            hiddenKey.type = 'hidden';
	    hiddenKey.id = 'key';
            hiddenKey.name = 'key'; // Name for the server-side
	}
    hiddenKey.value = cKey;

    alert('Hidden cfy #key = '+hiddenKey.value);
}

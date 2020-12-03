<p id='content'></p>
<textarea id='target'></textarea>

<script src="https://rawcdn.githack.com/brwhale/KataScript/81e245ec7a9c50a4a2f284b32df8f22354e7138f/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
function readLine(str) {
	var buffer = _malloc(str.length + 1);
	stringToUTF8(str, buffer, str.length + 1);
	_readLine(buffer);
	_free(buffer);
}

function displayInput(a) {
	document.getElementById('content').innerHTML += '> ' + a + '</br>';
}

document.getElementById('target').addEventListener('input', (event) => {
	var elem = document.getElementById('target');
  if (elem.value.endsWith('\n')) {
	  elem.value.split('\n').forEach(e => {displayInput(e); readLine(e);});
	  elem.value = '';
  }
});
</script>

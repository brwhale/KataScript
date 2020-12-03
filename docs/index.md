<p id='content'></p>
<textarea id='target'></textarea>

<script src="https://rawcdn.githack.com/brwhale/KataScript/9a97e72658e7081c967c5575c2df536e5b216ff5/jssrc/kscript.js" ></script>
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

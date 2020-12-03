<p id='content'></p>
<textarea id='target'/>

<script src="https://raw.githack.com/brwhale/KataScript/main/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
function readLine(str) {
	var buffer = _malloc(str.length + 1);
	stringToUTF8(str, buffer, str.length + 1);
	_readLine(buffer);
	_free(buffer);
}

document.getElementById('target').addEventListener('change', (event) => {
  if (event.target.value.endsWith('\n')) {
	  readLine(event.target.value);
	  document.getElementById('content').innerHTML = '';
  }
});
</script>

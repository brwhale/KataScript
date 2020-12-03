<p id='content'></p>
<p id='textarea'>default text here</p>
<p id='target'></p>

<script src="https://raw.githack.com/brwhale/KataScript/main/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
function readLine(str) {
	var buffer = _malloc(str.length + 1);
	stringToUTF8(str, buffer, str.length + 1);
	_readLine(buffer);
	_free(buffer);
}
</script>

<p id='content' class='content-area common'></p>
<textarea id='target' class='common'></textarea>

<script src="https://rawcdn.githack.com/brwhale/KataScript/2b898873992c249555a62bcc763789925f279fae/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
function readLine(str) {
	var buffer = _malloc(str.length + 1);
	stringToUTF8(str, buffer, str.length + 1);
	_readLine(buffer);
	_free(buffer);
}

function displayInput(a) {
  var inp = document.getElementById('content');
	inp.innerHTML += '> ' + a + '</br>';
  inp.scrollTop = inp.scrollHeight;
}

document.getElementById('target').addEventListener('input', (event) => {
	var elem = document.getElementById('target');
  if (elem.value.endsWith('\n')) {
	  elem.value.split('\n').forEach(e => {displayInput(e); readLine(e);});
	  elem.value = '';
  }
});
</script>
<style>
  .common {
    padding: 1em;
    margin: 0;
    width: 100%;
    background-color: antiquewhite;
  }
  textarea { 
    height: 1em;
    resize: none;
    overflow: hidden;
    }
  .content-area{    
    height: 500px;
    overflow: auto;
  }
</style>

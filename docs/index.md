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
<p id='content' class='content-area common'></p>
<textarea id='target' class='common'></textarea>
<script src="https://rawcdn.githack.com/brwhale/KataScript/d04d11f584e8faff3da04eefabf69becfa1d6b32/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
function readLine(str) {
    var lengthBytes = lengthBytesUTF8(str)+1;
    var buffer = _malloc(lengthBytes);
    stringToUTF8(str, buffer, lengthBytes);
    _readLineLen(buffer, lengthBytes);
    _free(buffer);
}

function displayInput(inp, a) {
    inp.innerHTML += '> ' + a + '</br>';
    inp.scrollTop = inp.scrollHeight;
}

document.getElementById('target').addEventListener('input', (event) => {
    var elem = document.getElementById('target');
    if (elem.value.endsWith('\n')) {
        var tempval = elem.value;
        elem.value = '';
	    tempval.split('\n').forEach(e => {
            if (e.length > 0) {
                var inp = document.getElementById('content');
                displayInput(inp, e); 
                readLine(e);
            }
        });	    
    }
});
</script>

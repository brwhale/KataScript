<style>
.common {
    padding: 1em;
    margin: 0;
    width: 100%;
    background-color: rgb(39, 44, 49);
    color:rgb(171, 214, 253); 
}
.input-area { 
    height: 1em;
    resize: none;
    overflow: hidden;
    border-style: inset;
    border-color: rgb(39,44,49);
    border-radius: 0px 0px 16px 16px;
    padding-right: .9em !important;
}
.content-area{    
    height: 500px;
    overflow: auto;
    border-radius: 16px 16px 0px 0px;
}
body {
    background-color: rgb(10, 11, 12);
}
a {
    color:rgb(95, 196, 255);
}
</style>
<p id='content' class='common content-area'></p>
<textarea id='target' class='common input-area'></textarea>
<script src="https://rawcdn.githack.com/brwhale/KataScript/d04d11f584e8faff3da04eefabf69becfa1d6b32/jssrc/kscript.js" ></script>
<script type="text/javascript"> 
let lines = Array();
let lineIndex = 0;

function readLine(str) {
    lines.push(str);
    lineIndex = lines.length;
    var lengthBytes = lengthBytesUTF8(str) + 1;
    var buffer = _malloc(lengthBytes);
    stringToUTF8(str, buffer, lengthBytes);
    _readLineLen(buffer, lengthBytes-1);
    _free(buffer);
}

function displayInput(inp, a) {
    inp.innerHTML += '> ' + a + '</br>';
    inp.scrollTop = inp.scrollHeight;
}

document.addEventListener('keydown', (event) => {
    var elem = document.getElementById('target');
    if (event.keyCode == 38) {
        // up
        lineIndex--;
        if (lineIndex < 0) {
            lineIndex = lines.length - 1;
        }
        elem.value = lines[lineIndex];
    } else if (event.keyCode == 40) {
        // down
        lineIndex++;
        if (lineIndex >= lines.length) {
            lineIndex = 0;
        }
        elem.value = lines[lineIndex];
    } else if (event.keyCode == 13) {
        // enter
        var tempval = elem.value.trim();
        elem.value = '';
        var inp = document.getElementById('content');
	    if (tempval.length > 0) {     
            displayInput(inp, tempval); 
            readLine(tempval);
        }
    }
});
</script>

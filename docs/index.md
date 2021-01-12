<style>
.common {
    width: 100%; 
    padding: 14px;
    margin: 0;
    background-color: rgb(39, 44, 49);
    color:rgb(171, 214, 253);
    white-space: pre-wrap;
    font-family: 'Fira Code', monospace;
}
.wrapper{
    position: relative;
    width: 100%;
}
.input-area { 
    position: absolute;
    height: 14px;
    resize: none;
    overflow: hidden;
    border-style: inset;
    border-color: rgb(39,44,49);
    border-radius: 0px 0px 16px 16px;
    padding-right: 10px;
    padding-top: 16px;
    padding-bottom: 12px;
    color:white;   /* sets the color of both caret and text */
    -webkit-text-fill-color: transparent; /* sets just the text color */
}
.content-area{   
    height: 500px;
    overflow: auto;
    border-radius: 16px 16px 0px 0px;
}
#fake-text {
    position: absolute;
    left: 0px;
    top: 0px;
    pointer-events: none;
    border-width: 2px;
    background-color: rgba(0,0,0,0) !important;
    padding-top: 7px;
    padding-bottom: 21px;
}
#target {
    font-size: 20px;
}
body {
    background-color: rgb(234, 225, 220);
    font-size: 20px;
}
a {
    color:rgb(95, 196, 255);
    -webkit-text-fill-color:rgb(95, 196, 255);
}
.highlight-comment {
    color:rgb(129, 129, 129); 
    -webkit-text-fill-color:rgb(129, 129, 129); 
}
.highlight-keyword {
    color:rgb(240, 88, 88); 
    -webkit-text-fill-color:rgb(240, 88, 88); 
}
.highlight-variable {
    color:rgb(124, 184, 240); 
    -webkit-text-fill-color:rgb(124, 184, 240); 
}
.highlight-number {
    color:rgb(134, 202, 96); 
    -webkit-text-fill-color:rgb(134, 202, 96); 
}
.highlight-operator {
    color:rgb(228, 94, 213);
    -webkit-text-fill-color:rgb(228, 94, 213);
}
.highlight-constant {
    color:rgb(0, 101, 253); 
    -webkit-text-fill-color:rgb(0, 101, 253); 
}
.highlight-string {
    color:rgb(221, 150, 19); 
    -webkit-text-fill-color:rgb(221, 150, 19); 
}
.highlight-function {
    color:rgb(238, 236, 129); 
    -webkit-text-fill-color:rgb(238, 236, 129); 
}
.highlight-dunno {
    color:white;
    -webkit-text-fill-color:white;
}
.highlight-hash {
    color:rgb(31, 126, 68);
    -webkit-text-fill-color:rgb(31, 126, 68);
}
span {
    background-color: inherit;
}
@media screen and (max-width: 960px) {
    footer {
        height: 600px;
        width: 100%;
    }
}
@media screen and (min-width: 960px) {
    footer {
        height: 60%;
        width: 35%;
    }
}
@media screen and (min-width: 960px) {
    section {
        width: 55%;
    }
}
iframe {
    height: 100%;
    width: 100%;
    padding-top: 10px;
    border:none;
}
</style>
<link rel="preconnect" href="https://fonts.gstatic.com">
<link href="https://fonts.googleapis.com/css2?family=Fira+Code:wght@500&display=swap" rel="stylesheet"> 
<div id="content" class="common content-area"></div>
<div class="wrapper">
<textarea id="target" spellcheck="false" class="common input-area" onfocus="this.value = this.value;"></textarea>
<div id="fake-text" class="common input-area"></div>
</div>

<script src="https://rawcdn.githack.com/brwhale/KataScript/5041d3dc9e52e3bd5b1c2f6d68d5cc59c762a15e/jssrc/kscript.js"></script>
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
    inp.innerHTML += '<span class="highlight-hash"># </span>' + a + '</br>';
    inp.scrollTop = inp.scrollHeight;
}

var constantsRegex = new RegExp('null|true|false');
var keywordsRegex = new RegExp('var|return|if|else|func|class');
var functionsRegex = new RegExp('bool|int|float|string|vec3|array|list|dictionary|print|typeof|getline|length|find|erase|sqrt|pow|abs|reverse|replace|startswith|endswith|split|contains|pushback|popback|popfront|back|front|min|max|map|fold|swap|range|sort|toarry|tolist|identity|listindex|applyfunction|copy|sin|cos|tan|clock|timesince|getduration');
var variableRegex = new RegExp('[a-zA-Z_]');

function findFirstInput(input, findchars, index) {
    for(var i = index; i < input.length; i++) {
        if (findchars.includes(input[i])) {
            return i;
        }
    }
    return -1;
}

var WhitespaceChars = " \t\n";
var MultiCharTokenStartChars = "+-/*<>=!&|";
var NumericChars = "0123456789.";
var NumericStartChars = "0123456789.-";
var GrammarChars = ' \t\n,.(){}[];+-/*%<>=!&|"';
function getTokens(input) {
    let exitFromComment = false;
    let leng = input.length;
    let ret = Array();
    if (len == 0) return ret;
    let pos = 0;
    let lpos = 0;
    while ((pos = findFirstInput(input, GrammarChars, lpos)) != -1) {
        var len = pos - lpos;
        if (input[pos] == '.' && pos + 1 < leng && NumericChars.includes(input[pos+1])) {
            pos = findFirstInput(input, GrammarChars, pos + 1);
            if (pos == -1) {
                ret.push([input.substring(lpos), "number"]);
                return ret;
            }
            ret.push([input.substr(lpos, pos - lpos), "number"]);
            lpos = pos;
            continue;
        }
        if (len) {
            var newstr = input.substr(lpos, pos - lpos);
            ret.push([newstr, newstr.match(constantsRegex) ? "constant" : newstr.match(keywordsRegex) ? "keyword" : newstr.match(functionsRegex) ? "function" : newstr.match(variableRegex) ? "variable" : "number"]);
            lpos = pos;
        } else {
            if (input[pos] == '\"' && pos > 0 && input[pos-1] != '\\') {
                var testpos = lpos + 1;
                var loop = true;
                while (loop) {						
                    pos = findFirstInput(input, '\"', testpos);
                    if (pos == -1) {
                        ret.push([input.substr(lpos), "string"]);
                        return ret;
                    }
                    loop = (input[pos - 1] == '\\');
                    testpos = pos + 1;
                }
                ret.push([input.substr(lpos, testpos - lpos), "string"]);
                lpos = testpos;
                continue;
            }
        }
        var lastchar = '';
        if (ret.length > 0) {
            lastchar = ret[ret.length - 1];
            lastchar = lastchar[lastchar[0].length -1];
        }
        if (input[pos] == '-' && NumericChars.includes(input[pos + 1]) 
            && (ret.length == 0 || MultiCharTokenStartChars.includes(lastchar))) {
            pos = findFirstInput(input, GrammarChars, pos + 1);
            if (pos == -1) {
                ret.push([input.substring(lpos), "number"]);
                return ret;
            }
            if (input[pos] == '.' && pos + 1 < leng && NumericChars.includes(input[pos + 1])) {
                pos = findFirstInput(input, GrammarChars, pos + 1);
            }
            if (pos == -1) {
                ret.push([input.substring(lpos), "number"]);
                return ret;
            }
            ret.push([input.substr(lpos, pos - lpos), "number"]);
            lpos = pos;
        } else if (WhitespaceChars.includes(input[pos])) {
            while (++pos < leng && WhitespaceChars.includes(input[pos])) {}
            ret.push([input.substr(lpos, pos - lpos), "whitespace"]);
            lpos = pos;
        } else {
            var stride = 1;
            if (MultiCharTokenStartChars.includes(input[pos]) && MultiCharTokenStartChars.includes(input[pos + 1])) {
                if (input[pos] == '/' && input[pos + 1] == '/') {
                    exitFromComment = true;
                    break;
                }
                ++stride;
            }
            ret.push([input.substr(lpos, stride), "operator"]);
            lpos += stride;
        }
    }
    if (lpos < input.length) {
        ret.push([input.substring(lpos), exitFromComment ? "comment" : "dunno"]);
    }
    return ret;
}

function highlightText(text) {
    var tokens = getTokens(text);
    var newtext = "";
    tokens.forEach(element => {
        newtext += '<span class="highlight-'+ element[1] +'">'+ element[0] +'</span>';
    });
    return newtext;
}

document.getElementById('target').addEventListener('input', (event) => {
    var elem = document.getElementById('target');
    while (elem.value.length > 0 && elem.value[0] == "\n") {
        elem.value = elem.value.substr(1);
    }
    document.getElementById('fake-text').innerHTML = highlightText(elem.value);
});

document.addEventListener('keydown', (event) => {
    var elem = document.getElementById('target');
    if (event.keyCode == 38) {
        // up
        lineIndex--;
        if (lineIndex < 0) {
            lineIndex = lines.length - 1;
        }
        elem.value = lines[lineIndex];
        document.getElementById('fake-text').innerHTML = highlightText(elem.value);
    } else if (event.keyCode == 40) {
        // down
        lineIndex++;
        if (lineIndex >= lines.length) {
            lineIndex = 0;
        }
        elem.value = lines[lineIndex];
        document.getElementById('fake-text').innerHTML = highlightText(elem.value);
    } else if (event.keyCode == 13) {
        // enter
        var clean = elem.value.split("\n");
        elem.value = '';
        var inp = document.getElementById('content');
        var syntaxxed = document.getElementById('fake-text').innerHTML.split("\n");
        for (var i = 0; i < clean.length; i++) {
            if (clean[i].length > 0) {     
                displayInput(inp, syntaxxed[i]); 
                readLine(clean[i]);
            }
        }   
    }
});

document.addEventListener('keyup', (event) => {
    if (event.keyCode == 38) {
        // up
        var elem = document.getElementById('target');
        elem.selectionStart = elem.selectionEnd = elem.value.length;
    }
});

document.getElementById('fake-text').innerHTML = highlightText(document.getElementById('target').value);

setTimeout(function(){ 
    var foot = document.getElementsByTagName("footer")[0];
    foot.innerHTML = '<iframe src="https://brwhale.github.io/KataScript/Readme"></iframe>' + foot.innerHTML;    
}, 300);

</script>

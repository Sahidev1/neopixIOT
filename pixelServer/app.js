
const express = require('express');
const bodyParser = require('body-parser');
const helpers = require('./helpers');

const app = express();


//const args = process.argv.slice(2);
//const IP = args[0] || "127.0.0.1";
//const PORT =  args[1] || "8000"; 
const PORT = process.env.PORT || 8000;
let colorReq = undefined;
let setcolor = undefined;
let lastSet = undefined;
let pollingrate = undefined;

let setpolling = undefined;
let setpreset = undefined;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }))

app.use(express.static(__dirname + '/public'));
app.set('view engine', 'ejs');

app.listen(PORT,/*IP,*/ (error) =>{
	if(!error)
		console.log("Server is Successfully Running," +
				"and App is listening on port ")
	else
		console.log("Error occurred, server can't start", error);
	}
);

app.get('/', (req, res) => {
    res.status(200);
    res.render('index', genVars());
});

app.get ('/unocheck', (req, res) =>{
    if (req.query.polling){
        pollingrate = req.query.polling;
        console.log(pollingrate);
    }
    if (setcolor){
        res.status(200);
        res.send("SETCOLOR=" + setcolor);
        lastSet = setcolor;
        setcolor = undefined;
    } else if(setpolling){
        res.status(200);
        res.send("SETPOLLING=" + setpolling);
        console.log("setpolling sent!");
        setpolling = undefined;
    } else if (setpreset){
        res.status(200);
        res.send("PRESET=" + setpreset);
        setpreset = undefined;
    }
    else {
        res.status(200);
        res.send("NOREQ");
    }
});

app.post('/setcolor', (req,res) => {
    colorReq = req.body;
    helpers.colorFilter(colorReq);
    setcolor = helpers.RGB_bitwise_encoding(colorReq);
    console.log(colorReq);
    console.log(setcolor);
    res.render('index', genVars());
});

app.post('/setspec', (req,res) => {
    specReq = req.body;
    setcolor = helpers.handleSpecColor(specReq) & (~0xff000000);
    console.log(specReq);
    console.log(setcolor);
    res.render('index',genVars());
});

app.post('/setpolling', (req,res) => {
    setpolling = helpers.pollingRateFilter(req.body.polling);
    console.log(setpolling);
    res.render('index',genVars());
});

app.post('/setpreset', (req,res) =>{
    if(req.body.preset){
        setpreset = req.body.preset;
    }
    console.log(setpreset);
    res.render('index', genVars());
})

function genVars (){
    var prop = {};
    if(pollingrate) prop.poll = pollingrate;
    else {
        prop.poll = "unknown";
    }
    return ({currcolor: helpers.RGB_bitwise_decoding(setcolor), lastcol: helpers.RGB_bitwise_decoding(lastSet),
    currpoll: prop});
}
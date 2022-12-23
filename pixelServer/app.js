
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

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }))

app.use(express.static('public'));

app.listen(PORT, (error) =>{
	if(!error)
		console.log("Server is Successfully Running," +
				"and App is listening on port ")
	else
		console.log("Error occurred, server can't start", error);
	}
);

app.get('/', (req, res) => {
    res.status(200);
    res.send("Henlo");
});

app.get ('/unocheck', (req, res) =>{
    if (setcolor){
        res.status(200);
        res.send("SETCOLOR=" + setcolor);
        setcolor = undefined;
    } else {
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
    res.redirect('/')
});

app.post('/setspec', (req,res) => {
    specReq = req.body;
    setcolor = helpers.handleSpecColor(specReq) & (~0xff000000);
    console.log(specReq);
    console.log(setcolor);
    res.redirect('/');
});
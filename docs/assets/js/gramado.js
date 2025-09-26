
// ------------------------
// The Gramado class.
class GramadoClass {
    constructor (s){
        console.log("GramadoClass: constructor()");
        console.log(s);
    }
    printString(s){
        console.log(s);
    }
    //...
};
let Gramado;
let __gramado_status = 0;
// ------------------------

function gramadoInitialize(){

    let ReturnValue = 1234;
    console.log("gramadoInitialize: Initializing ...");

// Creating the Gramado class.
    Gramado = new GramadoClass("Creating the Gramado object.");
    Gramado.printString("Testing Gramado.printString method ...");
    //...

    //arrowFunction1(1);
    //arrowFunction2(2);
    //arrowFunction3(3);
    //arrowFunction4();

    __gramado_status = 1;

// 1234
    return ReturnValue;
}

function gramadoGetStatus(){
    return __gramado_status;
}

function gramadoReloadPage(){
    window.location.reload();
}

function gramadoHelloWorld(){
    console.log("Hello")
    if (__gramado_status == 1){
        window.alert("Initialized");
    }else{
        window.alert("Not initialized");
    }
    return __gramado_status;
}

function gramadoVisitGramadoOnGithub(){
    window.open("https://github.com/gramado");
}

// ---------------------------------------

/* Learning ... */
let arrowFunction0 = (x,y) => {
    console.log("arrowFunction0:");
    console.log(x);
    console.log(y);
}

/* Learning ... */
let arrowFunction1 = (x) => {
    console.log("arrowFunction1:");
    console.log(x);
}

/* Learning ... */
let arrowFunction2 = (x) => console.log(x)

/* Learning ... */
let arrowFunction3 = x => console.log(x)

/* Learning ... */
let arrowFunction4 = () => console.log("arrowFunction4:")

// -----




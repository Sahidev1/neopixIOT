
function colorFilter (colProp){
    Object.keys(colProp).forEach (key => {
        if (colProp[key] > 255) colProp[key] = 255;
        if (colProp[key] < 0) colProp[key] = 0;
    })
}

function RGB_bitwise_encoding (colProp){
    let retval = 0;
    retval |= colProp.R;
    retval |= (colProp.G << 8);
    retval |= (colProp.B << 16);
    return retval;
}

function RGB_bitwise_decoding (rgbINT){
    if (rgbINT == undefined) return undefined;
    let mask = 0x000000ff;
    let colProp = {};
    colProp.R = rgbINT & mask;
    rgbINT >>= 8;
    colProp.G = rgbINT & mask;
    rgbINT >>= 8;
    colProp.B = rgbINT & mask;
    return colProp;
}

function handleSpecColor(specProp){
    let color=specProp.spec_color;
    let colProp = {};
    switch (color) {
        case "RED":
            colProp = {R:255, G:0, B:0};
            break;
        case "BLUE":
            colProp = {R:0, G:0, B: 255};
            break;
        case "GREEN":
            colProp = {R:0, G:255, B: 0};
            break;
        case "YELLOW":
            colProp = {R:255, G:255, B:0};
            break;
        case "PINK":
            colProp = {R:255, G:102, B:255};
            break;
        default:
            return {R:0, G:0, B:0};
    }
    return RGB_bitwise_encoding(colProp);
}

function pollingRateFilter (pollingrate){
    if (!pollingrate) return undefined;
    let defaultpooling = 2000;
    return pollingrate >= 250 && pollingrate <= 20000?pollingrate:defaultpooling;
}

module.exports = {colorFilter, RGB_bitwise_decoding , RGB_bitwise_encoding, handleSpecColor, pollingRateFilter}

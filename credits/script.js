// some kind of failure...was hoping I could do it without JS but the clipping path of the credit
// highlighters would not follow animation (I was probably just too dumb to do it properly :()
// tried CSS and animateMotion but fail. total disaster. very sad.
// so fall back to good ol javascript

// STOP! SCROLLERTIME!
const scrollerTime = 15000;

let yoyoTween = function(value, offsets, updateFn) {
    let yoyo = new TWEEN.Tween(value);
    let yoyoPath = Object.entries(offsets).reduce((obj, kv) => {
        obj[kv[0]] = kv[1].slice().reverse();
        // kill our current position
        obj[kv[0]].shift();
        // instead push original position
        obj[kv[0]].push(value[kv[0]]);
        return obj;
    }, {});
    yoyo.onUpdate(updateFn);
    yoyo.to(yoyoPath, 1000);
    console.log(JSON.stringify(yoyoPath));
    return yoyo;
}

let createTweenAlongPath = function(object, path, updateFn, segmentDuration) {
    let first; // bah
    let createTweensFromPath = (previousTween, segment) => {
        var tween = new TWEEN.Tween(object);
        tween.onUpdate(updateFn);
        if (previousTween) {
            previousTween.chain(tween);
        } else {
            // I don't get it...I am too dumb for the API
            first = tween;
        }
        tween.to(segment, segmentDuration);
        return tween;
    };

    let clone = path.map(v => Object.assign({}, v));
    // kill current position
    clone.pop();
    // instead add original position
    clone.reverse();
    clone.push(Object.assign({}, object));

    let forward = path.reduce(createTweensFromPath, undefined);
    let back = clone.reduce(createTweensFromPath, forward);
    back.chain(first);
    return first;
}

let animateSpot = function() {
    let spot = document.getElementById('spot');
    let c = {
        x: parseInt(spot.getAttribute('cx'), 10),
        y: parseInt(spot.getAttribute('cy'), 10),
    }

    let updateFn = function() {
        spot.setAttribute('cx', this.x);
        spot.setAttribute('cy', this.y);
    };

    let path = [
        {x: c.x+100, y: c.y+10},
        {x: c.x+240, y: c.y-70},
        {x: c.x+260, y: c.y+200},
        {x: c.x+310, y: c.y+90},
        {x: c.x+429, y: c.y+20},
        {x: c.x+550, y: c.y-20},
        {x: c.x+700, y: c.y+0},
        {x: c.x+801, y: c.y+110},
        {x: c.x+840, y: c.y-10},
        {x: c.x+970, y: c.y+0},
        {x: c.x+1170, y: c.y+90},
        {x: c.x+1210, y: c.y+140},
        {x: c.x+1300, y: c.y-20},
        {x: c.x+1410, y: c.y+0},
        {x: c.x+1620, y: c.y+101},
        {x: c.x+1770, y: c.y+50},
        {x: c.x+1810, y: c.y+80},
    ];

    var tween = createTweenAlongPath(c, path, updateFn, 500);
    tween.start();
}

let animateCredits = function() {
    let text = document.getElementById('credits');
    let c = {
        x: parseInt(text.getAttribute('x'), 10),
    }

    let updateFn = function() {
        text.setAttribute('x', this.x);
    };

    var tween = new TWEEN.Tween(c);
    tween.to({x: -15500}, scrollerTime);
    tween.onUpdate(updateFn);
    tween.start();
    return new Promise((resolve, reject) => {
        tween.onComplete(resolve);
    });
}

let animateAA = function() {
    let elements = Array.from(document.querySelectorAll('#aa > text'));
    elements.reverse();
    
    let updateFn = function(value) {
        elements.forEach((element, index) => {
            let angle = 2 * Math.PI / elements.length * index + 2 * Math.PI * value;
            element.setAttribute('x', Math.sin(angle) * 320);
            element.setAttribute('y', Math.cos(angle) * 320);
        });
    }

    var tween = new TWEEN.Tween(0);
    tween.to(1, 5000);
    tween.repeat(Infinity);
    tween.onUpdate(updateFn);
    tween.start();
}

let main = function() {
    let credits = document.getElementById('credits-greetings');
    let special = document.getElementById('special-thanks');
    animateSpot();
    animateAA();

    animateCredits()
    .then(() => {
        console.log('peng');
        return new Promise((resolve, reject) => {
            let fadeOut1 = new TWEEN.Tween(1);
            fadeOut1.to(0, 1000);
            fadeOut1.onUpdate(value => {
                credits.setAttribute('opacity', 1 - value);
            });
            fadeOut1.onComplete(resolve);
            fadeOut1.start();
        })
        .then(() => {
            return new Promise((resolve, reject) => {
                setTimeout(resolve, 1000);
            })
        })
        .then(() => {
            return new Promise((resolve, reject) => {
                let fadeIn = new TWEEN.Tween(1);
                fadeIn.to(0, 1000);
                fadeIn.onUpdate(value => {
                    special.setAttribute('opacity', value);
                });
                fadeIn.onComplete(resolve);
                fadeIn.start();
            })
        })
        .then(() => {
            return new Promise((resolve, reject) => {
                setTimeout(resolve, 5000);
            })
        })
        .then(() => {
            return new Promise((resolve, reject) => {
                let fadeOut1 = new TWEEN.Tween(1);
                fadeOut1.to(0, 1000);
                fadeOut1.onUpdate(value => {
                    special.setAttribute('opacity', 1 - value);
                });
                fadeOut1.onComplete(resolve);
                fadeOut1.start();
            })
        })
    })

    let animate = () => {
        requestAnimationFrame(animate);
        TWEEN.update();
    }
    animate();
}




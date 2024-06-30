/*
 Copyright (c) 2024 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

export class SpringCoefficients {
    declare posPos: number;
    declare posVel: number;
    declare velPos: number;
    declare velVel: number;

    constructor (posPos: number, posVel: number, velPos: number, velVel: number) {
        this.posPos = posPos;
        this.posVel = posVel;
        this.velPos = velPos;
        this.velVel = velVel;
    }

    set (posPos: number, posVel: number, velPos: number, velVel: number): typeof this {
        this.posPos = posPos;
        this.posVel = posVel;
        this.velPos = velPos;
        this.velVel = velVel;
        return this;
    }
}

export function getSpringCoefficients (out: SpringCoefficients, time: number, damping: number, speed: number): SpringCoefficients {
    // if time or speed is 0, then the spring won't move
    if (time === 0 || speed === 0) {
        return out.set(1, 0, 0, 1);
    }
    let posPos = 0;
    let posVel = 0;
    let velPos = 0;
    let velVel = 0;

    if (damping > 1) {
        // overdamped spring
        // solution to the characteristic equation:
        // z = -ζω ± Sqrt[ζ^2 - 1] ω
        // x[t] -> x0(e^(t z2) z1 - e^(t z1) z2)/(z1 - z2)
        //   + v0(e^(t z1) - e^(t z2))/(z1 - z2)
        // v[t] -> x0(z1 z2(-e^(t z1) + e^(t z2)))/(z1 - z2)
        //   + v0(z1 e^(t z1) - z2 e^(t z2))/(z1 - z2)

        const scaledTime = time * speed;
        const alpha = Math.sqrt(damping ** 2 - 1);
        const scaledInvAlpha = -0.5 / alpha;
        const z1 = -alpha - damping;
        const z2 = 1 / z1;
        const expZ1 = Math.exp(scaledTime * z1);
        const expZ2 = Math.exp(scaledTime * z2);

        posPos = (expZ2 * z1 - expZ1 * z2) * scaledInvAlpha;
        posVel = (expZ1 - expZ2) * scaledInvAlpha / speed;
        velPos = (expZ2 - expZ1) * scaledInvAlpha * speed;
        velVel = (expZ1 * z1 - expZ2 * z2) * scaledInvAlpha;
    } else if (damping === 1) {
        // critically damped spring
        // x[t] -> x0(e^-tω)(1+tω) + v0(e^-tω)t
        // v[t] -> x0(t ω^2)(-e^-tω) + v0(1 - tω)(e^-tω)

        const scaledTime = time * speed;
        const expTerm = Math.exp(-scaledTime);

        posPos = expTerm * (1 + scaledTime);
        posVel = expTerm * time;
        velPos = expTerm * (-scaledTime * speed);
        velVel = expTerm * (1 - scaledTime);
    } else {
        // underdamped spring
        // factored out of the solutions to the characteristic equation:
        // α = Sqrt[1 - ζ^2]
        // x[t] -> x0(e^-tζω)(α Cos[tα] + ζω Sin[tα])/α
        //       + v0(e^-tζω)(Sin[tα])/α
        // v[t] -> x0(-e^-tζω)(α^2 + ζ^2 ω^2)(Sin[tα])/α
        //       + v0(e^-tζω)(α Cos[tα] - ζω Sin[tα])/α

        const scaledTime = time * speed;
        const alpha = Math.sqrt(1 - damping ^ 2);
        const invAlpha = 1 / alpha;
        const alphaTime = alpha * scaledTime;
        const expTerm = Math.exp(-scaledTime * damping);
        const sinTerm = expTerm * Math.sin(alphaTime);
        const cosTerm = expTerm * Math.cos(alphaTime);
        const sinInvAlpha = sinTerm * invAlpha;
        const sinInvAlphaDamp = sinInvAlpha * damping;

        posPos = sinInvAlphaDamp + cosTerm;
        posVel = sinInvAlpha;
        velPos = -(sinInvAlphaDamp * damping + sinTerm * alpha);
        velVel = cosTerm - sinInvAlphaDamp;
    }

    return out.set(posPos, posVel, velPos, velVel);
}

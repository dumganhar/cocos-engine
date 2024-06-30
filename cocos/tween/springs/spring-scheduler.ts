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

import { Spring } from './spring';
import { SpringCoefficients, getSpringCoefficients } from './spring-coefficients';

const EPSILON = 0.0001;
const coefficient = new SpringCoefficients(0, 0, 0, 0);

export class SpringScheduler {
    private _activeSprings = new Set<Spring<any, string>>();
    private _springsToSleep = new Set<Spring<any, string>>();
    private _lastUpdateTime = 0; // TODO:

    update (now: number): void {
        this._lastUpdateTime = now;

        for (const spring of this._activeSprings) {
            getSpringCoefficients(
                coefficient,
                this._lastUpdateTime - spring._lastSchedule,
                spring._currentDamping,
                spring._currentSpeed,
            );

            const positions = spring._springPositions;
            const velocities = spring._springVelocities;
            const startDisplacements = spring._startDisplacements;
            const startVelocities = spring._startVelocities;
            let isMoving = false;

            for (const index in spring._springGoals) {
                const goal = spring._springGoals[index];

                const oldDisplacement = startDisplacements[index];
                const oldVelocity = startVelocities[index];
                const newDisplacement = oldDisplacement * coefficient.posPos + oldVelocity * coefficient.posVel;
                const newVelocity = oldDisplacement * coefficient.velPos + oldVelocity * coefficient.velVel;

                if (Math.abs(newDisplacement) > EPSILON || Math.abs(newVelocity) > EPSILON) {
                    isMoving = true;
                }

                positions[index] = newDisplacement + goal;
                velocities[index] = newVelocity;
            }

            if (!isMoving) this._springsToSleep.add(spring);
        }

        for (const spring of this._springsToSleep) {
            this._activeSprings.delete(spring);
            spring._currentValue.set(spring._springGoals);
            spring.update();
        }

        for (const spring of this._activeSprings) {
            spring._currentValue.set(spring._springPositions);
            spring.update();
        }
    }

    add (spring: Spring<any, string>): void {
        spring._lastSchedule = this._lastUpdateTime;
        spring._startDisplacements = {};
        spring._startVelocities = {};
        for (const k in spring._springGoals) {
            const goal = spring._springGoals[k];
            spring._startDisplacements[k] = spring._springPositions[k] - goal;
            spring._startVelocities[k] = spring._springVelocities[k];
        }

        this._activeSprings.add(spring);
    }

    remove (spring: Spring<any, string>): void {
        this._activeSprings.delete(spring);
    }
}

export const springScheduler = new SpringScheduler();
